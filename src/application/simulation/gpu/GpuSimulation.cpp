#include "application/simulation/gpu/GpuSimulation.hpp"
#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/simulation/Cell.hpp"
#include "application/simulation/painting/painting.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/Shader.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/utils/error.hpp"
#include <ctime>
#include <iostream>

using engine::error;
using engine::errorGL;

static constexpr unsigned int PADDING = 2;

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

auto GpuSimulation::create(
  int width,
  int height
) -> std::expected<GpuSimulation, engine::Error> {
  if (width <= PADDING * 2 || height <= PADDING * 2) {
    return std::unexpected(error("dimensions to small"));
  }

  return GpuSimulation(width, height);
}

GpuSimulation::GpuSimulation(int width, int height)
  :m_width(width)
  ,m_height(height)
{}

auto GpuSimulation::onCreate(const engine::Context& applicationContext) -> std::expected<void, engine::Error> {
  srand(time(NULL));

  auto initSimulationResult = initSimulation();
  if (!initSimulationResult.has_value()) {
    return std::unexpected(error("failed to init simulation", initSimulationResult.error()));
  }

  auto initDrawingResult = initDrawing();
  if (!initDrawingResult.has_value()) {
    return std::unexpected(error("failed to init drawing", initDrawingResult.error()));
  }

  return {};
}

auto GpuSimulation::onUpdate(const engine::Context& applicationContext) -> std::expected<void, engine::Error> {
  if (applicationContext.mouseLeftPressed) {
    const Cell cell = tryMapIntoCell(applicationContext.keyboardLastKeyPressed)
      .value_or(Cell::AIR);
    paint(cell, applicationContext);
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

auto GpuSimulation::initSimulation() -> std::expected<void, engine::Error> {
  auto isGpuBigEndianResult = isGpuBigEndian();
  if (!isGpuBigEndianResult.has_value()) {
    return std::unexpected(error("failed to check if gpu is big endian", isGpuBigEndianResult.error()));
  }
  m_bufferValueStride = 4;
  m_bufferValueOffset = 3 * *isGpuBigEndianResult;

  m_buffer = std::vector<Cell>(m_width * m_height * 4, static_cast<Cell>(0));
  auto& buffer = reinterpret_cast<std::vector<uint8_t>&>(*m_buffer);

  for (int row = 0; row < m_height; ++row) {
    for (int col = 0; col < m_width; ++col) {
      int idx = (row * m_width + col) * m_bufferValueStride + m_bufferValueOffset;
      if (row < PADDING || row >= m_width - PADDING || col < PADDING || col >= m_width - PADDING) {
        buffer[idx] = static_cast<uint8_t>(Cell::PADDING);
      } else {
        buffer[idx] = static_cast<uint8_t>(Cell::AIR);
      }
    }
  }

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

  auto inputSSBO = engine::ShaderStorageBuffer::create(m_width * m_height * 4);
  if (!inputSSBO.has_value()) {
    return std::unexpected(error("failed to create input SSBO", inputSSBO.error()));
  }
  m_inputSSBO = std::move(*inputSSBO);
  m_inputSSBO->store(buffer);

  auto outputSSBO = engine::ShaderStorageBuffer::create(m_width * m_height * 4);
  if (!outputSSBO.has_value()) {
    return std::unexpected(error("failed to create output SSBO", outputSSBO.error()));
  }
  m_outputSSBO = std::move(*outputSSBO);
  m_outputSSBO->store(buffer);

  m_computeSpaceX = m_width - 2 * PADDING;
  m_computeSpaceY = m_height - 2 * PADDING;

  auto useProgramResult = m_simulationProgram->useProgram();
  if (!useProgramResult.has_value()) {
    return std::unexpected(error("failed to use simulation program", useProgramResult.error()));
  }

  std::expected<void, engine::Error> uniformResult;
  uniformResult = m_simulationProgram->setUniform("gridWidth", m_width);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform gridWidth\n\t" << uniformResult.error() << '\n';
  }
  uniformResult = m_simulationProgram->setUniform("gridHeight", m_height);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform gridHeight\n\t" << uniformResult.error() << '\n';
  }
  uniformResult = m_simulationProgram->setUniform("gridPadding", PADDING);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform gridPadding\n\t" << uniformResult.error() << '\n';
  }
  uniformResult = m_simulationProgram->setUniform("simulationTexture", 0);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform simulationTexture\n\t" << uniformResult.error() << '\n';
  }

  return {};
}

auto GpuSimulation::initDrawing() -> std::expected<void, engine::Error> {
  auto texture = engine::Texture::create(m_width, m_height);
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

auto GpuSimulation::paint(Cell cell, const engine::Context& applicationContext) -> void {
  auto& buffer = reinterpret_cast<std::vector<uint8_t>&>(*m_buffer);

  m_inputSSBO->load(buffer);

  auto scaleX = static_cast<float>(m_width) / static_cast<float>(applicationContext.framebufferWidth);
  auto scaleY = static_cast<float>(m_height) / static_cast<float>(applicationContext.framebufferHeight);

  auto posX = static_cast<unsigned int>(static_cast<float>(applicationContext.mousePosX) * scaleX);
  auto posY = static_cast<unsigned int>(static_cast<float>(applicationContext.mousePosY) * scaleY);

  ::paint(
    posX,
    posY,
    cell,
    *m_buffer,
    m_width,
    m_height,
    PADDING,
    m_bufferValueOffset,
    m_bufferValueStride
  );

  m_inputSSBO->store(buffer);
}