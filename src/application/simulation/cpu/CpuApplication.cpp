#include "application/simulation/cpu/CpuApplication.hpp"
#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/painting/PaintingBrush.hpp"
#include "application/simulation/Cell.hpp"
#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/simulation/cpu/utils.hpp"
#include "application/simulation/input/SimulationInputHandler.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/input/binding/MouseButton.hpp"
#include "engine/utils/error.hpp"
#include <cmath>
#include <cstdint>
#include <vector>

using std::make_shared;
using engine::error;
using engine::input::MouseButton;

constexpr int PADDING = 2;

auto CpuApplication::create(
  int width,
  int height
) -> std::expected<CpuApplication, engine::Error> {
  if (width <= PADDING * 2 || height <= PADDING * 2) {
    return std::unexpected(error("too small simulation dimensions"));
  }

  auto simulator = CpuSimulator::create(width, height);
  if (!simulator.has_value()) {
    return std::unexpected(error("failed to create simulator", simulator.error()));
  }

  return CpuApplication(std::move(*simulator), width, height);
}

CpuApplication::CpuApplication(
  CpuSimulator&& simulator,
  int width,
  int height
)
  :m_simulator(std::move(simulator))
  ,m_width(width)
  ,m_height(height)
{}

auto CpuApplication::onCreate(engine::EngineContext& context) -> std::expected<void, engine::Error> {
  auto initDrawingResult = initDrawing();
  if (!initDrawingResult.has_value()) {
    return std::unexpected(error("failed to init drawing", initDrawingResult.error()));
  }

  initSimulation();
  initPainting();
  initKeyboardCallback(context);

  return {};
}

auto CpuApplication::onUpdate(engine::EngineContext& context) -> std::expected<void, engine::Error> {
  const auto& input = context.inputSystem;

  const bool mouseLeftPressed = input.isMouseButtonPressed(MouseButton::LEFT);
  if (mouseLeftPressed) {
    (*m_paintingBrush)->paint(context, reinterpret_cast<std::vector<uint8_t>&>(m_bufferIn));
  }

  m_simulator.run(m_bufferIn, m_bufferOut);

  mapCellsToColors(m_bufferOut, m_textureBuffer);

  auto storeResult = m_drawingTexture->store(m_textureBuffer);
  if(!storeResult.has_value()) {
    return std::unexpected(error("failed to store data in the drawing texture", storeResult.error()));
  }

  auto drawResult = m_drawingProgram->draw(*m_drawingTexture);
  if (!drawResult.has_value()) {
    return std::unexpected(error("failed to draw texture", drawResult.error()));
  }

  std::swap(m_bufferIn, m_bufferOut);

  return {};
}

auto CpuApplication::initDrawing() -> std::expected<void, engine::Error> {
  auto texture = engine::Texture::create(m_width, m_height);
  if (!texture.has_value()) {
    return std::unexpected(error("failed to create texture", texture.error()));
  }
  m_drawingTexture = std::move(*texture);

  auto program = TextureDrawingProgram::create();
  if (!program.has_value()) {
    return std::unexpected(error("failed to create drawing program", program.error()));
  }
  m_drawingProgram = std::move(*program);

  return {};
}

auto CpuApplication::initSimulation() -> void {
  m_bufferIn = std::vector<Cell>(m_width * m_height, Cell::PADDING);
  m_bufferOut = std::vector<Cell>(m_width * m_height, Cell::PADDING);
  for (int row = PADDING; row < m_height - PADDING; ++row) {
    for (int col = PADDING; col < m_width - PADDING; ++col) {
      const int idx = row * m_width + col;
      m_bufferIn[idx] = Cell::AIR;
      m_bufferOut[idx] = Cell::AIR;
    }
  }

  m_textureBuffer = std::vector<GLfloat>(m_width * m_height * 4, 0.0f);
}

auto CpuApplication::initPainting() -> void {
  auto paintingBrush = PaintingBrush::create(
    4, // SAND
    m_width,
    m_height,
    PADDING,
    0,
    1
  );
  m_paintingBrush = make_shared<PaintingBrush>(std::move(paintingBrush));
}

auto CpuApplication::initKeyboardCallback(engine::EngineContext& context) -> void {
  auto& input = context.inputSystem;

  auto inputHandler = SimulationInputHandler::create(*m_paintingBrush);
  m_inputHandler = make_shared<SimulationInputHandler>(std::move(inputHandler));

  input.addKeyboardKeyCallback(*m_inputHandler);
}
