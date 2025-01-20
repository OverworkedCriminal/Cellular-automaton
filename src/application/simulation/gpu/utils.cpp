#include "application/simulation/gpu/utils.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/Shader.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/utils/error.hpp"

using std::vector;
using std::expected;
using std::unexpected;
using engine::Error;
using engine::error;
using engine::Shader;
using engine::Program;
using engine::ShaderStorageBuffer;

auto isGpuBigEndian() -> expected<bool, Error> {
  auto shaderResult = Shader::create_from_file(GL_COMPUTE_SHADER, "shaders/endianess.compute.glsl");
  if (!shaderResult.has_value()) {
    return unexpected(error("failed to create endianess shader", shaderResult.error()));
  }
  Shader& shader = *shaderResult;

  const vector<Shader*> shaders = { &shader };
  auto programResult = Program::create(shaders);
  if (!programResult.has_value()) {
    return unexpected(error("failed to create endianess program", programResult.error()));
  }
  Program& program = *programResult;

  auto useProgramResult = program.useProgram();
  if (!useProgramResult.has_value()) {
    return unexpected(error("failed to use endianess program", useProgramResult.error()));
  }

  constexpr auto SHADER_BUFFER_SIZE = 4;
  auto shaderBufferResult = ShaderStorageBuffer::create(SHADER_BUFFER_SIZE);
  if (!shaderBufferResult.has_value()) {
    return unexpected(error("failed to create endianess shader buffer", shaderBufferResult.error()));
  }
  ShaderStorageBuffer& shaderBuffer = *shaderBufferResult;
  
  auto bindBufferResult = shaderBuffer.bindBufferBase(0);
  if (!bindBufferResult.has_value()) {
    return unexpected(error("failed to bind endianess shader buffer", bindBufferResult.error()));
  }

  vector<uint8_t> shaderBufferBytes(SHADER_BUFFER_SIZE, 0);
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