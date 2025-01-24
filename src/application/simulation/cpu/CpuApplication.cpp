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
using engine::Error;
using engine::Size2D;
using engine::input::MouseButton;
using engine::Texture;
using engine::EngineContext;

auto CpuApplication::create(
  Size2D<uint32_t> size,
  uint32_t processorsCount
) -> std::expected<CpuApplication, Error> {
  if (size.width < 1 || size.height < 1) {
    return std::unexpected(error("too small simulation dimensions"));
  }

  auto simulator = CpuSimulator::create(size, processorsCount);
  if (!simulator.has_value()) {
    return std::unexpected(error("failed to create simulator", simulator.error()));
  }

  const Size2D<uint32_t> sizeWithPadding = {
    .width = size.width + 2 * PADDING_SIZE,
    .height = size.height + 2 * PADDING_SIZE
  };

  return CpuApplication(std::move(*simulator), sizeWithPadding);
}

CpuApplication::CpuApplication(
  CpuSimulator&& simulator,
  Size2D<uint32_t> sizeWithPadding
)
  :m_sizeWithPadding(sizeWithPadding)
  ,m_simulator(std::move(simulator))
{}

auto CpuApplication::onCreate(EngineContext& context) -> std::expected<void, Error> {
  auto initDrawingResult = initDrawing();
  if (!initDrawingResult.has_value()) {
    return std::unexpected(error("failed to init drawing", initDrawingResult.error()));
  }

  initSimulation();
  initPainting(context);

  return {};
}

auto CpuApplication::onUpdate(EngineContext& context) -> std::expected<void, Error> {
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

auto CpuApplication::initDrawing() -> std::expected<void, Error> {
  const auto [width, height] = m_sizeWithPadding;

  auto texture = Texture::create(width, height);
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
  const auto [width, height] = m_sizeWithPadding;

  m_bufferIn = std::vector<uint8_t>(width * height, cell::PADDING);
  m_bufferOut = std::vector<uint8_t>(width * height, cell::PADDING);
  for (uint32_t row = PADDING_SIZE; row < height - PADDING_SIZE; ++row) {
    for (uint32_t col = PADDING_SIZE; col < width - PADDING_SIZE; ++col) {
      const uint32_t idx = row * width + col;
      m_bufferIn[idx] = cell::AIR;
      m_bufferOut[idx] = cell::AIR;
    }
  }

  m_textureBuffer = std::vector<GLfloat>(width * height * 4, 0.0f);
}

auto CpuApplication::initPainting(EngineContext& context) -> void {
  m_applicationPainting = ApplicationPainting::create(
    context,
    {
      .size = m_sizeWithPadding,
      .paddingSize = PADDING_SIZE,
      .valueOffset = 0,
      .valueStride = 1
    }
  );
}

auto CpuApplication::paint(const EngineContext& context) -> void {
  m_applicationPainting->paint(context, m_bufferIn);
}