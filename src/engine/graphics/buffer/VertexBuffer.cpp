#include "engine/graphics/buffer/VertexBuffer.hpp"
#include "engine/utils/error.hpp"

namespace engine {

auto VertexBuffer::create(
  const std::array<GLfloat, 16>& data
) -> std::expected<VertexBuffer, Error> {
  GLenum glError;
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);

  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
  glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glBufferData", glError));
  }

  return VertexBuffer(buffer);
}

VertexBuffer::VertexBuffer(GLuint buffer)
  :m_buffer(buffer)
{}

VertexBuffer::VertexBuffer(VertexBuffer&& other) {
  m_buffer = other.m_buffer;
  other.m_buffer = 0;
}

VertexBuffer::~VertexBuffer() {
  if (m_buffer != 0) {
    glDeleteBuffers(1, &m_buffer);
  }
}

auto VertexBuffer::operator=(VertexBuffer&& other) -> VertexBuffer&{
  m_buffer = other.m_buffer;
  other.m_buffer = 0;
  return *this;
}

auto VertexBuffer::operator*() -> GLuint {
  return m_buffer;
}

auto VertexBuffer::bind() -> void {
  glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
}

}