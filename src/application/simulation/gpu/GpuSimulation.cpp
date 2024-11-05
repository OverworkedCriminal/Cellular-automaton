#include "application/simulation/gpu/GpuSimulation.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/Shader.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/utils/error.hpp"
#include <iostream>

using engine::error;
using engine::errorGL;

static constexpr int PADDING = 2;

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

  std::vector<std::byte> shaderBufferBytes(SHADER_BUFFER_SIZE, std::byte(0));
  shaderBuffer.store(shaderBufferBytes);

  // start compute shader
  glDispatchCompute(1, 1, 1);

  // wait for compute shader to finish
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  shaderBuffer.load(shaderBufferBytes);

  // Shader stores value 1 to shaderBuffer on 4 bytes.
  // If GPU uses little endian value 1 should be at byteIdx 0
  // If GPU uses big endian value 1 should be at byteIdx 3
  std::byte byte = shaderBufferBytes[3];
  bool isBigEndian = byte == std::byte(1);

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

auto GpuSimulation::paint(const std::function<void(SimulationGrid&)>& paintFn) -> void {
  m_inputSSBO->load(*m_buffer);

  paintFn(*m_simulationGrid);

  m_inputSSBO->store(*m_buffer);
}

auto GpuSimulation::onCreate() -> std::expected<void, engine::Error> {
  auto isGpuBigEndianResult = isGpuBigEndian();
  if (!isGpuBigEndianResult.has_value()) {
    return std::unexpected(error("failed to check if gpu is big endian", isGpuBigEndianResult.error()));
  }

  auto initSimulationBufferResult = initSimulationGrid(*isGpuBigEndianResult);
  if (!initSimulationBufferResult.has_value()) {
    return std::unexpected(error("failed to init simulation buffer", initSimulationBufferResult.error()));
  }

  auto initSimulationResult = initSimulation();
  if (!initSimulationResult.has_value()) {
    return std::unexpected(error("failed to init simulation", initSimulationResult.error()));
  }

  return {};
}

auto GpuSimulation::onUpdate() -> std::expected<void, engine::Error> {
  auto useSimulationProgramResult = m_simulationProgram->useProgram();
  if (!useSimulationProgramResult.has_value()) {
    return std::unexpected(error("failed to use simulation program", useSimulationProgramResult.error()));
  }

  auto inputBindResult = m_inputSSBO->bindBufferBase(0);
  if (!inputBindResult.has_value()) {
    return std::unexpected(error("failed to bind input SSBO", inputBindResult.error()));
  }
  auto outputBindResult = m_outputSSBO->bindBufferBase(1);
  if (!outputBindResult.has_value()) {
    return std::unexpected(error("failed to bind output SSBO", outputBindResult.error()));
  }

  GLenum glError;
  glDispatchCompute(m_computeSpaceX, m_computeSpaceY, 1);
  glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glDispatchCompute", glError));
  }

  glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
  glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glMemoryBarrier", glError));
  }

  std::swap(*m_inputSSBO, *m_outputSSBO);

  return {};
}

auto GpuSimulation::initSimulationGrid(bool isGpuBigEndian) -> std::expected<void, engine::Error> {
  m_buffer = std::move(std::vector<std::byte>(m_width * m_height * 4, std::byte(0)));

  // GPU simulation values are stored on 4 byte uints.
  // Using single bytes would cause too many data races.
  auto simulationGridResult = SimulationGrid::create(
    &*m_buffer,
    m_width,
    m_height,
    4,
    3 * isGpuBigEndian
  );
  if (!simulationGridResult.has_value()) {
    return std::unexpected(error("failed to create simulation grid", simulationGridResult.error()));
  }
  m_simulationGrid = std::move(*simulationGridResult);

  return {};
}

/**
 * @brief
 * Relies on m_buffer so should be called after initSimulationGrid
 * 
 * @return std::expected<void, engine::Error> 
 */
auto GpuSimulation::initSimulation() -> std::expected<void, engine::Error> {
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
  m_inputSSBO->store(*m_buffer);

  auto outputSSBO = engine::ShaderStorageBuffer::create(m_width * m_height * 4);
  if (!outputSSBO.has_value()) {
    return std::unexpected(error("failed to create output SSBO", outputSSBO.error()));
  }
  m_outputSSBO = std::move(*outputSSBO);
  m_outputSSBO->store(*m_buffer);

  constexpr GLuint GRID_PADDING = 2;
  m_computeSpaceX = m_width - 2 * GRID_PADDING;
  m_computeSpaceY = m_height - 2 * GRID_PADDING;

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
  uniformResult = m_simulationProgram->setUniform("gridPadding", GRID_PADDING);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform gridPadding\n\t" << uniformResult.error() << '\n';
  }
  uniformResult = m_simulationProgram->setUniform("simulationTexture", 0);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform simulationTexture\n\t" << uniformResult.error() << '\n';
  }

  return {};
}