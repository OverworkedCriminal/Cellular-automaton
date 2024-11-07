#include "application/Application.hpp"
#include "application/simulation/ISimulation.hpp"
#include "engine/graphics/buffer/VertexArrayObject.hpp"
#include "engine/utils/error.hpp"

using engine::error;
using engine::errorGL;

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
{}

auto Application::onCreate(
  const engine::Context& context
) -> std::expected<void, engine::Error> {
  auto initDrawingResult = initDrawing();
  if (!initDrawingResult.has_value()) {
    return std::unexpected(error("initDrawing failed", initDrawingResult.error()));
  }

  auto simulationResult = m_simulation->onCreate();
  if (!simulationResult.has_value()) {
    return std::unexpected(error("simulation onCreate failed", simulationResult.error()));
  }

  return {};
}

auto Application::onDestroy(
  const engine::Context& context
) -> std::expected<void, engine::Error> {
  auto simulationResult = m_simulation->onDestroy();
  if (!simulationResult.has_value()) {
    return std::unexpected(error("simulation onDestroy failed", simulationResult.error()));
  }

  return {};
}

auto Application::onUpdate(
  const engine::Context& context
) -> std::expected<void, engine::Error> {
  if (context.mousePressed) {
    m_simulation->paint([&context, this](SimulationGrid& simulationGrid) {
      const int& x = context.mousePosX;
      const int& y = m_height - context.mousePosY;

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
          simulationGrid.setCell(currentCol, currentRow, std::byte(2));
        }
      }
    });
  }
  
  auto simulationResult = m_simulation->onUpdate();
  if (!simulationResult.has_value()) {
    return std::unexpected(error("simulation onUpdate failed", simulationResult.error()));
  }

  m_vao->bind();
  m_texture->bind();
  auto useProgramResult = m_drawingProgram->useProgram();
  if (!useProgramResult.has_value()) {
    return std::unexpected(error("use drawing program failed", useProgramResult.error()));
  }
  auto bindImageTextureResult = m_texture->bindImageTexture();
  if (!bindImageTextureResult.has_value()) {
    return std::unexpected(error("bind image texture failed", bindImageTextureResult.error()));
  }

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  const GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glDrawArrays", glError));
  }

  return {};
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
  m_drawingProgram = std::move(*drawingProgramResult);

  auto textureResult = engine::Texture::create(m_width, m_height);
  if (!textureResult.has_value()) {
    return std::unexpected(error("failed to create texture", textureResult.error()));
  }
  m_texture = std::move(*textureResult);

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
  m_vbo = std::move(*vboResult);

  const std::vector<engine::VaoAttribute> vaoAttributes = {
    {
      .offset = 0 * sizeof(GLfloat),
      .stride = 4 * sizeof(GLfloat),
      .size = 2,
      .buffer = *m_vbo
    },
    {
      .offset = 2 * sizeof(GLfloat),
      .stride = 4 * sizeof(GLfloat),
      .size = 2,
      .buffer = *m_vbo
    }
  };
  auto vaoResult = engine::VertexArrayObject::create(vaoAttributes);
  if (!vaoResult.has_value()) {
    return std::unexpected(error("create VAO failed", vaoResult.error()));
  }
  m_vao = std::move(*vaoResult);

  return {};
}
