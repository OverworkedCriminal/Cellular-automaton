#include "application/simulation/gpu/GpuApplication.hpp"
#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/painting/ApplicationPainting.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/simulation/cell.hpp"
#include "application/simulation/gpu/GpuSimulator.hpp"
#include "application/simulation/gpu/utils.hpp"
#include "application/simulation/padding.hpp"
#include "engine/EngineContext.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/input/binding/MouseButton.hpp"
#include "engine/utils/error.hpp"
#include <ctime>

using engine::error;
using engine::Error;
using engine::EngineContext;
using engine::input::MouseButton;
using engine::Size2D;
using engine::ShaderStorageBuffer;
using engine::Texture;

auto GpuApplication::create(
  Size2D<uint32_t> size
) -> std::expected<GpuApplication, Error> {
  if (size.width < 1 || size.height < 1) {
    return std::unexpected(error("dimensions to small"));
  }

  const Size2D<uint32_t> sizeWithPadding = {
    .width = size.width + 2 * PADDING_SIZE,
    .height = size.height + 2 * PADDING_SIZE
  };

  return GpuApplication(sizeWithPadding);
}

GpuApplication::GpuApplication(Size2D<uint32_t> sizeWithPadding)
  :m_sizeWithPadding(sizeWithPadding)
{}

auto GpuApplication::onCreate(EngineContext& context) -> std::expected<void, Error> {
  auto& input = context.inputSystem;
  
  srand(time(NULL));

  auto isGpuBigEndianResult = isGpuBigEndian();
  if (!isGpuBigEndianResult.has_value()) {
    return std::unexpected(error("failed to check GPU endianess", isGpuBigEndianResult.error()));
  }

  auto initBufferResult = initBuffer(*isGpuBigEndianResult);
  if (!initBufferResult.has_value()) {
    return std::unexpected(error("failed to init buffer", initBufferResult.error()));
  }

  auto initSimulationResult = initSimulation();
  if (!initSimulationResult.has_value()) {
    return std::unexpected(error("failed to init simulation", initSimulationResult.error()));
  }

  auto initDrawingResult = initDrawing();
  if (!initDrawingResult.has_value()) {
    return std::unexpected(error("failed to init drawing", initDrawingResult.error()));
  }

  initPainting(context, *isGpuBigEndianResult);

  return {};
}

auto GpuApplication::onUpdate(EngineContext& context) -> std::expected<void, Error> {
  const auto& input = context.inputSystem;

  const bool mouseLeftPressed = input.isMouseButtonPressed(MouseButton::LEFT);
  if (mouseLeftPressed) {
    paint(context);
  }

  auto runResult = m_simulator->run(*m_inputSSBO, *m_outputSSBO, *m_drawingTexture);
  if (!runResult.has_value()) {
    return std::unexpected(error("failed to run simulator", runResult.error()));
  }

  auto bindTextureResult = m_drawingTexture->bindImageTexture(0);
  if (!bindTextureResult.has_value()) {
    return std::unexpected(error("failed to bind image texture", bindTextureResult.error()));
  }

  auto drawResult = m_drawingProgram->draw(*m_drawingTexture);
  if (!drawResult.has_value()) {
    return std::unexpected(error("failed to draw texture", drawResult.error()));
  }

  std::swap(*m_inputSSBO, *m_outputSSBO);

  return {};
}

auto GpuApplication::initBuffer(bool isGpuBigEndian) -> std::expected<void, Error> {
  const auto [width, height] = m_sizeWithPadding;
  const uint32_t offset = 3 * isGpuBigEndian;
  const uint32_t stride = 4;

  m_buffer = std::vector<cell_t>(width * height, 0);

  for (uint32_t row = 0; row < height; ++row) {
    for (uint32_t col = 0; col < width; ++col) {
      uint32_t idx = (row * width + col) * stride + offset;
      if (row < PADDING_SIZE || row >= width - PADDING_SIZE || col < PADDING_SIZE || col >= width - PADDING_SIZE) {
        m_buffer[idx] = cell::PADDING;
      } else {
        m_buffer[idx] = cell::AIR;
      }
    }
  }

  return {};
}

auto GpuApplication::initSimulation() -> std::expected<void, Error> {
  auto simulatorResult = GpuSimulator::create({
    .width = m_sizeWithPadding.width - 2 * PADDING_SIZE,
    .height = m_sizeWithPadding.height - 2 * PADDING_SIZE
  });
  if (!simulatorResult.has_value()) {
    return std::unexpected(error("failed to create GpuSimulator", simulatorResult.error()));
  }
  m_simulator = std::move(*simulatorResult);

  const auto [width, height] = m_sizeWithPadding;

  auto inputSSBO = ShaderStorageBuffer::create(width * height * 4);
  if (!inputSSBO.has_value()) {
    return std::unexpected(error("failed to create input SSBO", inputSSBO.error()));
  }
  m_inputSSBO = std::move(*inputSSBO);
  m_inputSSBO->store(m_buffer);

  auto outputSSBO = ShaderStorageBuffer::create(width * height * 4);
  if (!outputSSBO.has_value()) {
    return std::unexpected(error("failed to create output SSBO", outputSSBO.error()));
  }
  m_outputSSBO = std::move(*outputSSBO);
  m_outputSSBO->store(m_buffer);

  return {};
}

auto GpuApplication::initDrawing() -> std::expected<void, Error> {
  const auto [width, height] = m_sizeWithPadding;

  auto texture = Texture::create(width, height);
  if (!texture.has_value()) {
    return std::unexpected(error("failed to create texture", texture.error()));
  }
  m_drawingTexture = std::move(*texture);

  auto drawingProgram = TextureDrawingProgram::create();
  if (!drawingProgram.has_value()) {
    return std::unexpected(error("failed to create drawing program", drawingProgram.error()));
  }
  m_drawingProgram = std::move(*drawingProgram);

  return {};
}

auto GpuApplication::initPainting(
  EngineContext& context,
  bool isGpuBigEndian
) -> void {
  PaintingCanvasDescription canvasDescription = {
    .size = m_sizeWithPadding,
    .paddingSize = PADDING_SIZE
  };

  m_applicationPainting = ApplicationPainting::create(context, canvasDescription);
}

auto GpuApplication::paint(const EngineContext& context) -> void {
  m_inputSSBO->load(m_buffer);
  m_applicationPainting->paint(context, m_buffer);
  m_inputSSBO->store(m_buffer);
}
