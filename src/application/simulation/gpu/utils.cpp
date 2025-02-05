#include "application/simulation/gpu/utils.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/Shader.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/utils/error.hpp"
#include <bit>

using engine::Error;
using engine::error;
using engine::Shader;
using engine::Program;
using engine::ShaderStorageBuffer;

auto isCpuAndGpuEndianessMatching() -> std::expected<bool, Error> {
  auto shaderResult = Shader::create_from_file(GL_COMPUTE_SHADER, "shaders/endianess.compute.glsl");
  if (!shaderResult.has_value()) {
    return std::unexpected(error("failed to create endianess shader", shaderResult.error()));
  }
  Shader& shader = *shaderResult;

  const std::vector<Shader*> shaders = { &shader };
  auto programResult = Program::create(shaders);
  if (!programResult.has_value()) {
    return std::unexpected(error("failed to create endianess program", programResult.error()));
  }
  Program& program = *programResult;

  auto useProgramResult = program.useProgram();
  if (!useProgramResult.has_value()) {
    return std::unexpected(error("failed to use endianess program", useProgramResult.error()));
  }

  constexpr auto SHADER_BUFFER_SIZE = 4;
  auto shaderBufferResult = ShaderStorageBuffer::create(SHADER_BUFFER_SIZE);
  if (!shaderBufferResult.has_value()) {
    return std::unexpected(error("failed to create endianess shader buffer", shaderBufferResult.error()));
  }
  ShaderStorageBuffer& shaderBuffer = *shaderBufferResult;
  
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

  // Map array of bytes to single uint32_t
  const uint8_t* shaderBufferBytesData = shaderBufferBytes.data();
  const uint32_t* storedNumberPtr = reinterpret_cast<const uint32_t*>(shaderBufferBytesData);

  // If one stored on GPU matches 1 on CPU endianess match
  const bool isEndianessMatching = (*storedNumberPtr) == 1;

  return isEndianessMatching;
}

auto switchEndianess(std::vector<uint32_t>& buffer) -> void {
  for (uint32_t i = 0; i < buffer.size(); ++i) {
    buffer[i] = std::byteswap(buffer[i]);
  }
}
