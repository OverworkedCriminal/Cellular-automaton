#include "application/simulation/gpu/GpuApplication.hpp"
#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/painting/PaintingBrush.hpp"
#include "application/painting/painting.hpp"
#include "application/simulation/cell.hpp"
#include "application/simulation/padding.hpp"
#include "engine/EngineContext.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/Shader.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/input/binding/MouseButton.hpp"
#include "engine/utils/error.hpp"
#include <ctime>
#include <iostream>

using std::make_shared;
using engine::error;
using engine::errorGL;
using engine::EngineContext;
using engine::input::MouseButton;

static auto isGpuBigEndian() -> std::expected<bool, engine::Error> {
  auto shaderResult = engine::Shader::create_from_file(GL_COMPUTE_SHADER, "shaders/endianess.compute.glsl");
  if (!shaderResult.has_value()) {
    return std::unexpected(error("failed to create endianess shader", shaderResult.error()));
  }
  engine::Shader& shader = *shaderResult;

  const std::vector<engine::Shader*> shaders = { &shader };
  auto programResult = engine::Program::create(shaders);
  if (!programResult.has_value()) {
    return std::unexpected(error("failed to create endianess program", programResult.error()));
  }
  engine::Program& program = *programResult;

  auto useProgramResult = program.useProgram();
  if (!useProgramResult.has_value()) {
    return std::unexpected(error("failed to use endianess program", useProgramResult.error()));
  }

  constexpr auto SHADER_BUFFER_SIZE = 4;
  auto shaderBufferResult = engine::ShaderStorageBuffer::create(SHADER_BUFFER_SIZE);
  if (!shaderBufferResult.has_value()) {
    return std::unexpected(error("failed to create endianess shader buffer", shaderBufferResult.error()));
  }
  engine::ShaderStorageBuffer& shaderBuffer = *shaderBufferResult;
  
  auto bindBufferResult = shaderBuffer.bindBufferBase(0);
  if (!bindBufferResult.has_value()) {
    return std::unexpected(error("failed to bind endianess shader buffer", bindBufferResult.error()));
  }

  std::vector<uint8_t> shaderBufferBytes(SHADER_BUFFER_SIZE, 0);
  shaderBuffer.store(shaderBufferBytes);

  // start compute shader
  glDispatchCompute(1, 1, 1);

  // wait for compute shader to finish
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  shaderBuffer.load(shaderBufferBytes);

  // Shader stores value 1 to shaderBuffer on 4 bytes.
  // If GPU uses little endian value 1 should be at byteIdx 0
  // If GPU uses big endian value 1 should be at byteIdx 3
  uint8_t byte = shaderBufferBytes[3];
  bool isBigEndian = byte == 1;

  return isBigEndian;
}

auto GpuApplication::create(
  int width,
  int height
) -> std::expected<GpuApplication, engine::Error> {
  if (width <= PADDING_SIZE * 2 || height <= PADDING_SIZE * 2) {
    return std::unexpected(error("dimensions to small"));
  }

  return GpuApplication(width, height);
}

GpuApplication::GpuApplication(uint32_t width, uint32_t height)
  :m_canvasDescription({
    .size = {
      .width = width,
      .height = height
    },
    .paddingSize = PADDING_SIZE
  })
{}

auto GpuApplication::onCreate(EngineContext& context) -> std::expected<void, engine::Error> {
  auto& input = context.inputSystem;
  
  srand(time(NULL));

  auto initBufferResult = initBuffer();
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

  initPainting(context);

  return {};
}

auto GpuApplication::onUpdate(EngineContext& context) -> std::expected<void, engine::Error> {
  const auto& input = context.inputSystem;

  const bool mouseLeftPressed = input.isMouseButtonPressed(MouseButton::LEFT);
  if (mouseLeftPressed) {
    paint(context);
  }

  auto useSimulationProgramResult = m_simulationProgram->useProgram();
  if (!useSimulationProgramResult.has_value()) {
    return std::unexpected(error("failed to use simulation program", useSimulationProgramResult.error()));
  }
  auto uniformResult = m_simulationProgram->setUniform(
    "priorityDirection",
    (rand() % 2) * 2 - 1
  );
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set priorityDirection uniform\n\t" << uniformResult.error() << '\n';
  }

  auto inputBindResult = m_inputSSBO->bindBufferBase(0);
  if (!inputBindResult.has_value()) {
    return std::unexpected(error("failed to bind input SSBO", inputBindResult.error()));
  }
  auto outputBindResult = m_outputSSBO->bindBufferBase(1);
  if (!outputBindResult.has_value()) {
    return std::unexpected(error("failed to bind output SSBO", outputBindResult.error()));
  }
  auto bindImageTextureResult = m_drawingTexture->bindImageTexture(0);
  if (!bindImageTextureResult.has_value()) {
    return std::unexpected(error("failed to bind image texture", bindImageTextureResult.error()));
  }

  GLenum glError;
  glDispatchCompute(m_computeSpaceX, m_computeSpaceY, 1);
  glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glDispatchCompute", glError));
  }

  glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
  glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glMemoryBarrier", glError));
  }

  auto drawResult = m_drawingProgram->draw(*m_drawingTexture);
  if (!drawResult.has_value()) {
    return std::unexpected(error("failed to draw texture", drawResult.error()));
  }

  std::swap(*m_inputSSBO, *m_outputSSBO);

  return {};
}

auto GpuApplication::initBuffer() -> std::expected<void, engine::Error> {
  auto isGpuBigEndianResult = isGpuBigEndian();
  if (!isGpuBigEndianResult.has_value()) {
    return std::unexpected(error("failed to check if gpu is big endian", isGpuBigEndianResult.error()));
  }

  const auto [width, height] = m_canvasDescription.size;
  const uint32_t offset = 3 * *isGpuBigEndianResult;
  const uint32_t stride = 4;
  m_canvasDescription.valueOffset = offset;
  m_canvasDescription.valueStride = stride;

  m_buffer = std::vector<uint8_t>(width * height * 4, 0);

  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      int idx = (row * width + col) * stride + offset;
      if (row < PADDING_SIZE || row >= width - PADDING_SIZE || col < PADDING_SIZE || col >= width - PADDING_SIZE) {
        m_buffer[idx] = cell::PADDING;
      } else {
        m_buffer[idx] = cell::AIR;
      }
    }
  }

  return {};
}

auto GpuApplication::initSimulation() -> std::expected<void, engine::Error> {
  auto simulationShaderResult = engine::Shader::create_from_file(GL_COMPUTE_SHADER, "shaders/simulation.compute.glsl");
  if (!simulationShaderResult.has_value()) {
    return std::unexpected(error("failed to create compute shader", simulationShaderResult.error()));
  }

  const std::vector<engine::Shader*> shaders = { &*simulationShaderResult };
  auto simulationProgram = engine::Program::create(shaders);
  if (!simulationProgram.has_value()) {
    return std::unexpected(error("failed to create simulation program", simulationProgram.error()));
  }
  m_simulationProgram = std::move(*simulationProgram);

  const auto [width, height] = m_canvasDescription.size;

  auto inputSSBO = engine::ShaderStorageBuffer::create(width * height * 4);
  if (!inputSSBO.has_value()) {
    return std::unexpected(error("failed to create input SSBO", inputSSBO.error()));
  }
  m_inputSSBO = std::move(*inputSSBO);
  m_inputSSBO->store(m_buffer);

  auto outputSSBO = engine::ShaderStorageBuffer::create(width * height * 4);
  if (!outputSSBO.has_value()) {
    return std::unexpected(error("failed to create output SSBO", outputSSBO.error()));
  }
  m_outputSSBO = std::move(*outputSSBO);
  m_outputSSBO->store(m_buffer);

  m_computeSpaceX = width - 2 * PADDING_SIZE;
  m_computeSpaceY = height - 2 * PADDING_SIZE;

  auto useProgramResult = m_simulationProgram->useProgram();
  if (!useProgramResult.has_value()) {
    return std::unexpected(error("failed to use simulation program", useProgramResult.error()));
  }

  std::expected<void, engine::Error> uniformResult;
  uniformResult = m_simulationProgram->setUniform("gridWidth", width);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform gridWidth\n\t" << uniformResult.error() << '\n';
  }
  uniformResult = m_simulationProgram->setUniform("gridHeight", height);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform gridHeight\n\t" << uniformResult.error() << '\n';
  }
  uniformResult = m_simulationProgram->setUniform("gridPadding", PADDING_SIZE);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform gridPadding\n\t" << uniformResult.error() << '\n';
  }
  uniformResult = m_simulationProgram->setUniform("simulationTexture", 0);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform simulationTexture\n\t" << uniformResult.error() << '\n';
  }

  return {};
}

auto GpuApplication::initDrawing() -> std::expected<void, engine::Error> {
  const auto [width, height] = m_canvasDescription.size;
  
  auto texture = engine::Texture::create(width, height);
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

auto GpuApplication::initPainting(engine::EngineContext& context) -> void {
  const auto paintingBrush = PaintingBrush::create(1, cell::SAND);
  m_paintingBrush = make_shared<PaintingBrush>(std::move(paintingBrush));

  auto inputHandler = PaintingBrushCallbacksHandler::create(*m_paintingBrush);
  m_inputHandler = make_shared<PaintingBrushCallbacksHandler>(std::move(inputHandler));

  context.inputSystem.addKeyboardKeyCallback(*m_inputHandler);
}

auto GpuApplication::paint(const EngineContext& context) -> void {
  m_inputSSBO->load(m_buffer);

  auto position = mapWindowPositionToSimulationPosition(
    context.inputSystem.getMousePosition(),
    context.windowSystem.getFramebufferSize(),
    m_canvasDescription.size
  );
  (*m_paintingBrush)->paint(m_buffer, m_canvasDescription, position);

  m_inputSSBO->store(m_buffer);
}
