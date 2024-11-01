#include "application/gpu/GpuApplication.hpp"
#include "engine/graphics/buffer/VertexArrayObject.hpp"
#include "engine/graphics/buffer/VertexBuffer.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/Shader.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "glad/glad.h"
#include <optional>
#include <vector>

auto GpuApplication::create(
  int width,
  int height
) -> std::expected<GpuApplication, std::string> {
  // Simulation uses 5x5 windows to determine state of current cell
  // so it's necessary to add padding of 2 on each side of simulation
  if (width <= 4 || height <= 4) {
    return std::unexpected("invalid simulation dimensions: minimum supported size: 5x5");
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

static auto isGpuLittleEndian() -> std::expected<bool, std::string> {
  auto shaderResult = engine::Shader::create_from_file(GL_COMPUTE_SHADER, "shaders/endianess.compute.glsl");
  if (!shaderResult.has_value()) {
    return std::unexpected("failed to create endianess shader: " + shaderResult.error());
  }
  engine::Shader& shader = *shaderResult;

  const std::vector<engine::Shader*> shaders = { &shader };
  auto programResult = engine::Program::create(shaders);
  if (!programResult.has_value()) {
    return std::unexpected("failed to create endianess program: " + programResult.error());
  }
  engine::Program& program = *programResult;

  auto useProgramResult = program.useProgram();
  if (!useProgramResult.has_value()) {
    return std::unexpected("failed to use endianess program: " + useProgramResult.error());
  }

  constexpr auto SHADER_BUFFER_SIZE = 4;
  auto shaderBufferResult = engine::ShaderStorageBuffer::create(SHADER_BUFFER_SIZE);
  if (!shaderBufferResult.has_value()) {
    return std::unexpected("failed to create endianess shader buffer: " + shaderBufferResult.error());
  }
  engine::ShaderStorageBuffer& shaderBuffer = *shaderBufferResult;
  
  auto bindBufferResult = shaderBuffer.bindBufferBase(0);
  if (!bindBufferResult.has_value()) {
    return std::unexpected("failed to bind endianess shader buffer: " + bindBufferResult.error());
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

static auto createDrawingProgram() -> std::expected<engine::Program, std::string> {
  auto vertexShaderResult = engine::Shader::create_from_file(GL_VERTEX_SHADER, "shaders/texture.vertex.glsl");
  if (!vertexShaderResult.has_value()) {
    return std::unexpected("failed to create vertex shader from file: " + vertexShaderResult.error());
  }
  engine::Shader& vertexShader = *vertexShaderResult;

  auto fragmentShaderResult = engine::Shader::create_from_file(GL_FRAGMENT_SHADER, "shaders/texture.fragment.glsl");
  if (!fragmentShaderResult.has_value()) {
    return std::unexpected("failed to create fragment shader from file: " + fragmentShaderResult.error());
  }
  engine::Shader& fragmentShader = *fragmentShaderResult;

  const std::vector<engine::Shader*> shaders = { &vertexShader, &fragmentShader };

  auto programResult = engine::Program::create(shaders);
  if (!programResult.has_value()) {
    return std::unexpected("failed to create drawing program: " + programResult.error());
  }

  return programResult;
}

auto GpuApplication::initDrawing() -> std::expected<void, std::string> {
  m_vao = engine::VertexArrayObject::create();
  m_vao->bind();

  auto drawingProgramResult = createDrawingProgram();
  if (!drawingProgramResult.has_value()) {
    return std::unexpected("createDrawingProgram failed: " + drawingProgramResult.error());
  }
  m_drawingProgram = std::move(*drawingProgramResult);

  auto textureResult = engine::Texture::create(m_width, m_height);
  if (!textureResult.has_value()) {
    return std::unexpected("failed to create texture: " + textureResult.error());
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
    return std::unexpected("createVertexBuffer failed: " + vertexBufferResult.error());
  }
  m_vertexBuffer = std::move(*vertexBufferResult);

  return {};
}

auto GpuApplication::initSimulation() -> std::expected<void, std::string> {
  auto isLittleEndianResult = isGpuLittleEndian();
  if (!isLittleEndianResult.has_value()) {
    return std::unexpected("failed to check GPU endianess: " + isLittleEndianResult.error());
  }

  return {};
}

auto GpuApplication::onCreate(
  const Context& context
) -> std::expected<void, std::string> {
  auto initDrawingResult = initDrawing();
  if (!initDrawingResult.has_value()) {
    return std::unexpected("failed to init drawing: " + initDrawingResult.error());
  }

  auto initSimulationResult = initSimulation();
  if (!initDrawingResult.has_value()) {
    return std::unexpected("failed to init simulation: " + initSimulationResult.error());
  }

  m_vao->bind();
  m_vertexBuffer->bind();
  m_texture->bind();

  return {};
}

auto GpuApplication::onDestroy(
  const Context& context
) -> std::expected<void, std::string> {
  return {};
}

auto GpuApplication::onUpdate(
  const Context& context
) -> std::expected<void, std::string> {
  auto useProgramResult = m_drawingProgram->useProgram();
  if (!useProgramResult.has_value()) {
    return std::unexpected("failed to use drawing program: " + useProgramResult.error());
  }

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  return {};
}