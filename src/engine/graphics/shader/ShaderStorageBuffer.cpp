#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/utils/error.hpp"
#include <algorithm>

namespace engine {

auto ShaderStorageBuffer::create(
  GLsizeiptr size
) -> std::expected<ShaderStorageBuffer, Error> {
  if (size <= 0) {
    return std::unexpected(error("ShaderStorageBuffer must have positive size"));
  }

  // makes sure size is multiple of 4
  const GLsizeiptr effectiveSize = size + size % 4;

  GLuint ssbo;
  glCreateBuffers(1, &ssbo);
  glNamedBufferData(ssbo, effectiveSize, NULL, GL_DYNAMIC_READ);

  const GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    glDeleteBuffers(1, &ssbo);
    return std::unexpected(errorGL("glNamedBufferData", glError));
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

auto ShaderStorageBuffer::bindBufferBase(GLuint index) -> std::expected<void, Error> {
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_ssbo);
  const GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glBindBufferBase", glError));
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