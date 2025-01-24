#include "application/simulation/gpu/GpuSimulator.hpp"
#include "application/simulation/padding.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/Shader.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/utils/error.hpp"
#include <iostream>
#include <vector>

using engine::Error;
using engine::Shader;
using engine::Program;
using engine::Size2D;
using engine::error;
using engine::errorGL;
using engine::ShaderStorageBuffer;
using engine::Texture;

auto GpuSimulator::create(
  Size2D<uint32_t> size
) -> std::expected<GpuSimulator, Error> {
  if (size.width < 1 || size.height < 1) {
    return std::unexpected(error("invalid simulation dimensions"));
  }

  auto simulationShaderResult = Shader::create_from_file(GL_COMPUTE_SHADER, "shaders/simulation.compute.glsl");
  if (!simulationShaderResult.has_value()) {
    return std::unexpected(error("failed to create compute shader", simulationShaderResult.error()));
  }

  const std::vector<Shader*> shaders = { &*simulationShaderResult };
  auto program = Program::create(shaders);
  if (!program.has_value()) {
    return std::unexpected(error("failed to create simulation program", program.error()));
  }

  auto useProgramResult = program->useProgram();
  if (!useProgramResult.has_value()) {
    return std::unexpected(error("failed to use simulation program", useProgramResult.error()));
  }

  std::expected<void, Error> uniformResult;
  uniformResult = program->setUniform("gridWidth", size.width + 2 * PADDING_SIZE);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform gridWidth\n\t" << uniformResult.error() << '\n';
  }
  uniformResult = program->setUniform("gridHeight", size.height + 2 * PADDING_SIZE);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform gridHeight\n\t" << uniformResult.error() << '\n';
  }
  uniformResult = program->setUniform("gridPadding", PADDING_SIZE);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform gridPadding\n\t" << uniformResult.error() << '\n';
  }
  uniformResult = program->setUniform("simulationTexture", 0);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set uniform simulationTexture\n\t" << uniformResult.error() << '\n';
  }

  return GpuSimulator(std::move(*program), size);
}

GpuSimulator::GpuSimulator(Program&& program, Size2D<GLuint> computeSpace)
  :m_program(std::move(program))
  ,m_computeSpace(computeSpace)
  ,m_priorityDirection(-1)
{}

auto GpuSimulator::run(
  ShaderStorageBuffer& input,
  ShaderStorageBuffer& output,
  Texture& outputTexture
) -> std::expected<void, Error> {
  m_priorityDirection = -m_priorityDirection;

  auto useProgramResult = m_program.useProgram();
  if (!useProgramResult.has_value()) {
    return std::unexpected(error("failed to use program", useProgramResult.error()));
  }

  auto uniformResult = m_program.setUniform("priorityDirection", m_priorityDirection);
  if (!uniformResult.has_value()) {
    std::cerr << "failed to set priorityDirection uniform\n\t" << uniformResult.error() << '\n';
  }

  auto inputBindResult = input.bindBufferBase(0);
  if (!inputBindResult.has_value()) {
    return std::unexpected(error("failed to bind input SSBO", inputBindResult.error()));
  }
  auto outputBindResult = output.bindBufferBase(1);
  if (!outputBindResult.has_value()) {
    return std::unexpected(error("failed to bind output SSBO", outputBindResult.error()));
  }
  auto bindImageTextureResult = outputTexture.bindImageTexture(0);
  if (!bindImageTextureResult.has_value()) {
    return std::unexpected(error("failed to bind image texture", bindImageTextureResult.error()));
  }

  GLenum glError;
  glDispatchCompute(m_computeSpace.width, m_computeSpace.height, 1);
  glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glDispatchCompute", glError));
  }

  glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
  glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glMemoryBarrier", glError));
  }

  return {};
}