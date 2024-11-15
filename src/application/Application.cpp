#include "application/Application.hpp"
#include "application/simulation/ISimulation.hpp"
#include "engine/application/KeyboardKey.hpp"
#include "engine/application/MouseButton.hpp"
#include "engine/graphics/buffer/VertexArrayObject.hpp"
#include "engine/utils/error.hpp"
#include <cstdlib>
#include <functional>

using engine::error;
using engine::errorGL;

constexpr auto CV_AIR   = std::byte(2);
constexpr auto CV_SAND  = std::byte(4);
constexpr auto CV_WATER = std::byte(8);

auto Application::create(
  int width,
  int height,
  std::unique_ptr<ISimulation> simulation
) -> std::expected<Application, engine::Error> {
  if (width <= 0 || height <= 0) {
    return std::unexpected(error("invalid width or height"));
  }

  return Application(width, height, std::move(simulation));
}

Application::Application(
  int width,
  int height,
  std::unique_ptr<ISimulation>&& simulation
)
  :m_width(width)
  ,m_height(height)
  ,m_simulation(std::forward<std::unique_ptr<ISimulation>&&>(simulation))
  ,m_paintFn([](auto grid) {})
{}

auto Application::onCreate() -> std::expected<void, engine::Error> {
  srand(std::time(0));

  m_mouseLeftPressed = false;
  m_paintFn = std::bind(&Application::paintSquare, this, std::placeholders::_1);

  m_simulationWidthScale = static_cast<float>(m_simulation->width()) / static_cast<float>(m_width);
  m_simulationHeightScale = static_cast<float>(m_simulation->height()) / static_cast<float>(m_height);

  auto initDrawingResult = initDrawing();
  if (!initDrawingResult.has_value()) {
    return std::unexpected(error("initDrawing failed", initDrawingResult.error()));
  }

  m_context.texturePtr = *m_texturePtrOpt;

  auto simulationResult = m_simulation->onCreate(m_context);
  if (!simulationResult.has_value()) {
    return std::unexpected(error("simulation onCreate failed", simulationResult.error()));
  }

  m_selectedCellValue = CV_SAND;

  return {};
}

auto Application::onDestroy() -> std::expected<void, engine::Error> {
  auto simulationResult = m_simulation->onDestroy(m_context);
  if (!simulationResult.has_value()) {
    return std::unexpected(error("simulation onDestroy failed", simulationResult.error()));
  }

  return {};
}

auto Application::onUpdate() -> std::expected<void, engine::Error> {
  if (m_mouseLeftPressed) {
    m_simulation->paint(m_paintFn);
  }

  updateContext();

  auto simulationResult = m_simulation->onUpdate(m_context);
  if (!simulationResult.has_value()) {
    return std::unexpected(error("simulation onUpdate failed", simulationResult.error()));
  }

  m_vaoOpt->bind();
  (*m_texturePtrOpt)->bind();
  auto useProgramResult = m_drawingProgramOpt->useProgram();
  if (!useProgramResult.has_value()) {
    return std::unexpected(error("use drawing program failed", useProgramResult.error()));
  }

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  const GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glDrawArrays", glError));
  }

  return {};
}

auto Application::onKeyboardInput(engine::KeyboardKey key, bool pressed) -> void {
  if (pressed == false) {
    return;
  }

  switch (key) {
    case engine::KeyboardKey::_1: m_selectedCellValue = CV_AIR;   break;
    case engine::KeyboardKey::_2: m_selectedCellValue = CV_SAND;  break;
    case engine::KeyboardKey::_3: m_selectedCellValue = CV_WATER; break;
    default: break;
  }
}

auto Application::onMouseMoveInput(unsigned int posX, unsigned int posY) -> void {
  m_mousePosX = posX;
  m_mousePosY = posY;
}

auto Application::onMouseButtonInput(engine::MouseButton button, bool pressed) -> void {
  if (button != engine::MouseButton::LEFT) {
    return;
  }

  m_mouseLeftPressed = pressed;
}


auto Application::initDrawing() -> std::expected<void, engine::Error> {
  auto vertexShaderResult = engine::Shader::create_from_file(GL_VERTEX_SHADER, "shaders/texture.vertex.glsl");
  if (!vertexShaderResult.has_value()) {
    return std::unexpected(error("failed to create vertex shader from file", vertexShaderResult.error()));
  }

  auto fragmentShaderResult = engine::Shader::create_from_file(GL_FRAGMENT_SHADER, "shaders/texture.fragment.glsl");
  if (!fragmentShaderResult.has_value()) {
    return std::unexpected(error("failed to create fragment shader from file", fragmentShaderResult.error()));
  }

  const std::vector<engine::Shader*> shaders = {
    &*vertexShaderResult,
    &*fragmentShaderResult
  };
  auto drawingProgramResult = engine::Program::create(shaders);
  if (!drawingProgramResult.has_value()) {
    return std::unexpected(error("failed to create drawing program", drawingProgramResult.error()));
  }
  m_drawingProgramOpt = std::move(*drawingProgramResult);

  auto textureResult = engine::Texture::create(m_simulation->width(), m_simulation->height());
  if (!textureResult.has_value()) {
    return std::unexpected(error("failed to create texture", textureResult.error()));
  }
  m_texturePtrOpt = std::make_shared<engine::Texture>(std::move(*textureResult));

  const std::array<GLfloat, 16> data = {
    // positions  tex coords
    -1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f
  };
  auto vboResult = engine::VertexBuffer::create(data);
  if (!vboResult.has_value()) {
    return std::unexpected(error("create VBO failed", vboResult.error()));
  }
  m_vboOpt = std::move(*vboResult);

  const std::vector<engine::VaoAttribute> vaoAttributes = {
    {
      .offset = 0 * sizeof(GLfloat),
      .stride = 4 * sizeof(GLfloat),
      .size = 2,
      .buffer = *m_vboOpt
    },
    {
      .offset = 2 * sizeof(GLfloat),
      .stride = 4 * sizeof(GLfloat),
      .size = 2,
      .buffer = *m_vboOpt
    }
  };
  auto vaoResult = engine::VertexArrayObject::create(vaoAttributes);
  if (!vaoResult.has_value()) {
    return std::unexpected(error("create VAO failed", vaoResult.error()));
  }
  m_vaoOpt = std::move(*vaoResult);

  return {};
}

auto Application::updateContext() -> void {
  m_context.priorityDirection = static_cast<Direction>((rand() % 2) * 2 - 1);
}

auto Application::paintSquare(SimulationGrid& simulationGrid) -> void {
  const auto x = static_cast<unsigned int>(static_cast<float>(m_mousePosX) * m_simulationWidthScale);
  const auto y = static_cast<unsigned int>(static_cast<float>(m_height - m_mousePosY) * m_simulationHeightScale);

  if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
    return;
  }

  for (int col = -5; col <= 5; ++col) {
    const int currentCol = x + col;
    if (currentCol < 0 || currentCol >= m_width) {
      continue;
    }

    for (int row = -5; row <= 5; ++row) {
      const int currentRow = y + row;
      if (currentRow < 0 || currentRow >= m_height) {
        continue;
      }
      simulationGrid.setCell(currentCol, currentRow, m_selectedCellValue);
    }
  }
}