#include "application/simulation/cpu/CpuApplication.hpp"
#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/painting/ApplicationPainting.hpp"
#include "application/simulation/cell.hpp"
#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/simulation/cpu/utils.hpp"
#include "application/simulation/padding.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/input/binding/MouseButton.hpp"
#include "engine/utils/error.hpp"
#include <cmath>
#include <cstdint>
#include <vector>

using engine::error;
using engine::input::MouseButton;

auto CpuApplication::create(
  int width,
  int height
) -> std::expected<CpuApplication, engine::Error> {
  if (width <= PADDING_SIZE * 2 || height <= PADDING_SIZE * 2) {
    return std::unexpected(error("too small simulation dimensions"));
  }

  auto simulator = CpuSimulator::create({
    .width = width - 2 * PADDING_SIZE,
    .height = height - 2 * PADDING_SIZE
  });
  if (!simulator.has_value()) {
    return std::unexpected(error("failed to create simulator", simulator.error()));
  }

  return CpuApplication(std::move(*simulator), width, height);
}

CpuApplication::CpuApplication(
  CpuSimulator&& simulator,
  uint32_t width,
  uint32_t height
)
  :m_size({ .width = width, .height = height })
  ,m_simulator(std::move(simulator))
{}

auto CpuApplication::onCreate(engine::EngineContext& context) -> std::expected<void, engine::Error> {
  auto initDrawingResult = initDrawing();
  if (!initDrawingResult.has_value()) {
    return std::unexpected(error("failed to init drawing", initDrawingResult.error()));
  }

  initSimulation();
  initPainting(context);

  return {};
}

auto CpuApplication::onUpdate(engine::EngineContext& context) -> std::expected<void, engine::Error> {
  const bool mouseLeftPressed = context.inputSystem.isMouseButtonPressed(MouseButton::LEFT);
  if (mouseLeftPressed) {
    paint(context);
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
  const auto [width, height] = m_size;

  auto texture = engine::Texture::create(width, height);
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
  const auto [width, height] = m_size;

  m_bufferIn = std::vector<uint8_t>(width * height, cell::PADDING);
  m_bufferOut = std::vector<uint8_t>(width * height, cell::PADDING);
  for (int row = PADDING_SIZE; row < height - PADDING_SIZE; ++row) {
    for (int col = PADDING_SIZE; col < width - PADDING_SIZE; ++col) {
      const int idx = row * width + col;
      m_bufferIn[idx] = cell::AIR;
      m_bufferOut[idx] = cell::AIR;
    }
  }

  m_textureBuffer = std::vector<GLfloat>(width * height * 4, 0.0f);
}

auto CpuApplication::initPainting(engine::EngineContext& context) -> void {
  m_applicationPainting = ApplicationPainting::create(
    context,
    {
      .size = m_size,
      .paddingSize = PADDING_SIZE,
      .valueOffset = 0,
      .valueStride = 1
    }
  );
}

auto CpuApplication::paint(const engine::EngineContext& context) -> void {
  m_applicationPainting->paint(context, m_bufferIn);
}