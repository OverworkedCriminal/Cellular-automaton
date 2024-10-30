#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include <algorithm>
#include <format>

namespace engine {

auto ShaderStorageBuffer::create(
  GLsizeiptr size
) -> std::expected<ShaderStorageBuffer, std::string> {
  if (size <= 0) {
    return std::unexpected("ShaderStorageBuffer must have positive size");
  }

  // makes sure size is multiple of 4
  const GLsizeiptr effectiveSize = size + size % 4;

  GLuint ssbo;
  glCreateBuffers(1, &ssbo);
  glNamedBufferData(ssbo, effectiveSize, NULL, GL_DYNAMIC_READ);

  const GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    glDeleteBuffers(1, &ssbo);
    return std::unexpected(std::format("glNamedBufferData failed: 0x{:04x}", error));
  }

  ShaderStorageBuffer shaderStorageBuffer(ssbo, size);

  return shaderStorageBuffer;
}

ShaderStorageBuffer::ShaderStorageBuffer(GLuint ssbo, GLsizeiptr size)
  :m_ssbo(ssbo)
  ,m_size(size)
{

}

ShaderStorageBuffer::ShaderStorageBuffer(ShaderStorageBuffer&& other) {
  m_ssbo = other.m_ssbo;
  other.m_ssbo = 0;
}

ShaderStorageBuffer::~ShaderStorageBuffer() {
  if (m_ssbo != 0) {
    glDeleteBuffers(1, &m_ssbo);
  }
}

auto ShaderStorageBuffer::operator=(ShaderStorageBuffer&& other) -> ShaderStorageBuffer& {
  m_ssbo = other.m_ssbo;
  other.m_ssbo = 0;
  return *this;
}

auto ShaderStorageBuffer::bindBufferBase(GLuint index) -> std::expected<void, std::string> {
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_ssbo);
  const GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    return std::unexpected(std::format("glBindBufferBase failed: 0x{:04x}", error));
  }

  return {};
}

auto ShaderStorageBuffer::store(const std::vector<GLubyte>& buffer) -> void {
  const GLsizeiptr size = std::min(
    m_size,
    static_cast<GLsizeiptr>(buffer.size())
  );

  glNamedBufferSubData(m_ssbo, 0, size, buffer.data());
}

auto ShaderStorageBuffer::load(std::vector<GLubyte>& buffer) -> void {
  const GLsizeiptr size = std::min(
    m_size,
    static_cast<GLsizeiptr>(buffer.size())
  );

  glGetNamedBufferSubData(m_ssbo, 0, size, buffer.data());
}

}