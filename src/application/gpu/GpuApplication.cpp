#include "application/gpu/GpuApplication.hpp"
#include "engine/graphics/buffer/VertexArrayObject.hpp"
#include "engine/graphics/buffer/VertexBuffer.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/Shader.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/utils/error.hpp"
#include "glad/glad.h"
#include <iostream>
#include <optional>
#include <vector>

using engine::error;
using engine::errorGL;

auto GpuApplication::create(
  int width,
  int height
) -> std::expected<GpuApplication, engine::Error> {
  // Simulation uses 5x5 windows to determine state of current cell
  // so it's necessary to add padding of 2 on each side of simulation
  if (width <= 4 || height <= 4) {
    return std::unexpected(error("invalid simulation dimensions: minimum supported size: 5x5"));
  }

  GpuApplication application(width, height);

  return application;
}

GpuApplication::GpuApplication(int width, int height)
  :m_width(width)
  ,m_height(height)
  ,m_drawingProgram(std::nullopt)
  ,m_simulationProgram(std::nullopt)
{}

GpuApplication::GpuApplication(GpuApplication&& other)
  :m_width(other.m_width)
  ,m_height(other.m_height)
  ,m_drawingProgram(std::move(other.m_drawingProgram))
  ,m_simulationProgram(std::move(other.m_simulationProgram))
{
  other.m_drawingProgram = std::nullopt;
  other.m_simulationProgram = std::nullopt;
}

auto GpuApplication::operator=(GpuApplication&& other) -> GpuApplication&{
  m_width = other.m_width;
  m_height = other.m_height;
  m_drawingProgram = std::move(other.m_drawingProgram);
  m_simulationProgram = std::move(other.m_simulationProgram);

  other.m_drawingProgram = std::nullopt;
  other.m_simulationProgram = std::nullopt;

  return *this;
}

static auto isGpuLittleEndian() -> std::expected<bool, engine::Error> {
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

  std::vector<GLubyte> shaderBufferBytes(SHADER_BUFFER_SIZE, 0);
  shaderBuffer.store(shaderBufferBytes);

  // start compute shader
  glDispatchCompute(1, 1, 1);

  // wait for compute shader to finish
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  shaderBuffer.load(shaderBufferBytes);

  // Shader stores value 1 to shaderBuffer on 4 bytes.
  // If GPU uses little endian value 1 should be at byteIdx 0
  // If GPU uses big endian value 1 should be at byteIdx 3
  GLuint byte = shaderBufferBytes[0];
  bool isLittleEndian = byte == 1;

  return isLittleEndian;
}

static auto isCpuLittleEndian() -> bool {
  const unsigned int value = 1;
  const unsigned char* valueBytes = reinterpret_cast<const unsigned char*>(&value);
  const bool isLittleEndian = valueBytes[0] == 1;
  return isLittleEndian;
}

auto GpuApplication::initDrawing() -> std::expected<void, engine::Error> {
  m_vao = engine::VertexArrayObject::create();
  m_vao->bind();

  auto vertexShaderResult = engine::Shader::create_from_file(GL_VERTEX_SHADER, "shaders/texture.vertex.glsl");
  if (!vertexShaderResult.has_value()) {
    return std::unexpected(error("failed to create vertex shader from file", vertexShaderResult.error()));
  }
  engine::Shader& vertexShader = *vertexShaderResult;

  auto fragmentShaderResult = engine::Shader::create_from_file(GL_FRAGMENT_SHADER, "shaders/texture.fragment.glsl");
  if (!fragmentShaderResult.has_value()) {
    return std::unexpected(error("failed to create fragment shader from file", fragmentShaderResult.error()));
  }
  engine::Shader& fragmentShader = *fragmentShaderResult;

  const std::vector<engine::Shader*> shaders = { &vertexShader, &fragmentShader };
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
  auto vertexBufferResult = engine::VertexBuffer::create(data);
  if (!vertexBufferResult.has_value()) {
    return std::unexpected(error("createVertexBuffer failed", vertexBufferResult.error()));
  }
  m_vertexBuffer = std::move(*vertexBufferResult);

  return {};
}

auto GpuApplication::initSimulation() -> std::expected<void, engine::Error> {
  auto isGpuLittleEndianResult = isGpuLittleEndian();
  if (!isGpuLittleEndianResult.has_value()) {
    return std::unexpected(error("failed to check GPU endianess", isGpuLittleEndianResult.error()));
  }
  const bool isGpuLE = *isGpuLittleEndianResult;
  const bool isCpuLE = isCpuLittleEndian();
  const bool endianessMatch = isGpuLE == isCpuLE;

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

  auto outputSSBO = engine::ShaderStorageBuffer::create(m_width * m_height * 4);
  if (!outputSSBO.has_value()) {
    return std::unexpected(error("failed to create output SSBO", outputSSBO.error()));
  }
  m_outputSSBO = std::move(*outputSSBO);

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
    std::cerr << "failed to set uniform gridWidth: " << uniformResult.error() << '\n';
  }
  uniformResult = m_simulationProgram->setUniform("gridHeight", m_height);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform gridHeight: " << uniformResult.error() << '\n';
  }
  uniformResult = m_simulationProgram->setUniform("gridPadding", GRID_PADDING);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform gridPadding: " << uniformResult.error() << '\n';
  }
  uniformResult = m_simulationProgram->setUniform("simulationTexture", 0);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform simulationTexture: " << uniformResult.error() << '\n';
  }

  return {};
}

auto GpuApplication::onCreate(
  const engine::Context& context
) -> std::expected<void, engine::Error> {
  auto initDrawingResult = initDrawing();
  if (!initDrawingResult.has_value()) {
    return std::unexpected(error("failed to init drawing", initDrawingResult.error()));
  }

  auto initSimulationResult = initSimulation();
  if (!initSimulationResult.has_value()) {
    return std::unexpected(error("failed to init simulation: ", initSimulationResult.error()));
  }

  m_vao->bind();
  m_vertexBuffer->bind();
  m_texture->bind();
  auto bindImageTextureResult = m_texture->bindImageTexture();
  if (!bindImageTextureResult.has_value()) {
    return std::unexpected(error("failed to bind image texture: ", bindImageTextureResult.error()));
  }

  return {};
}

auto GpuApplication::onDestroy(
  const engine::Context& context
) -> std::expected<void, engine::Error> {
  return {};
}

auto GpuApplication::onUpdate(
  const engine::Context& context
) -> std::expected<void, engine::Error> {
  GLenum glError;

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

  auto useDrawingProgramResult = m_drawingProgram->useProgram();
  if (!useDrawingProgramResult.has_value()) {
    return std::unexpected(error("failed to use drawing program", useDrawingProgramResult.error()));
  }

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  std::swap(*m_inputSSBO, *m_outputSSBO);

  return {};
}