#include "engine/graphics/buffer/VertexBuffer.hpp"
#include <format>


namespace engine {

auto VertexBuffer::create(
  const std::array<GLfloat, 16>& data
) -> std::expected<VertexBuffer, std::string> {
  GLenum error;
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) 0);
  error = glGetError();
  if (error != GL_NO_ERROR) {
    return std::unexpected(std::format("glVertexAttribPointer failed: 0x{:04x}", error));
  }
  glEnableVertexAttribArray(0);
  error = glGetError();
  if (error != GL_NO_ERROR) {
    return std::unexpected(std::format("glEnableVertexAttribArray failed: 0x{:04x}", error));
  }

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));
  error = glGetError();
  if (error != GL_NO_ERROR) {
    return std::unexpected(std::format("glVertexAttribPointer failed: 0x{:04x}", error));
  }
  glEnableVertexAttribArray(1);
  error = glGetError();
  if (error != GL_NO_ERROR) {
    return std::unexpected(std::format("glEnableVertexAttribArray failed: 0x{:04x}", error));
  }

  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
  error = glGetError();
  if (error != GL_NO_ERROR) {
    return std::unexpected(std::format("glBufferData failed: 0x{:04x}", error));
  }

  VertexBuffer outBuffer(buffer);

  return outBuffer;
}

VertexBuffer::VertexBuffer(GLuint buffer)
  :m_buffer(buffer)
{

}

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