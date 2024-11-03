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

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) 0);
  glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glVertexAttribPointer", glError));
  }
  glEnableVertexAttribArray(0);
  glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glEnableVertexAttribArray", glError));
  }

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));
  glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glVertexAttribPointer", glError));
  }
  glEnableVertexAttribArray(1);
  glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glEnableVertexAttribArray", glError));
  }

  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
  glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glBufferData", glError));
  }

  VertexBuffer outBuffer(buffer);

  return outBuffer;
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

auto VertexBuffer::bind() -> void {
  glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
}

}