#include "engine/graphics/buffer/VertexArrayObject.hpp"
#include "engine/graphics/buffer/VertexBuffer.hpp"
#include "engine/utils/error.hpp"

namespace engine {

auto VertexArrayObject::create(
  const std::vector<VaoAttribute>& attributes
) -> std::expected<VertexArrayObject, Error> {
  if (attributes.empty()) {
    return std::unexpected(error("cannot create VAO without any attribute"));
  }

  GLenum glError;
  GLuint vao;
  glCreateVertexArrays(1, &vao);

  for (GLuint i = 0; i < attributes.size(); ++i) {
    const VaoAttribute& attribute = attributes[i];
    if (attribute.size <= 0 || attribute.size > 4) {
      glDeleteVertexArrays(1, &vao);
      return std::unexpected(error("attribute size must be 1,2,3,4"));
    }

    glVertexArrayAttribFormat(vao, i, attribute.size, attribute.type, attribute.normalized, 0);
    glError = glGetError();
    if (glError != GL_NO_ERROR) {
      glDeleteVertexArrays(1, &vao);
      return std::unexpected(errorGL("glVertexArrayAttribFormat", glError));
    }

    glVertexArrayAttribBinding(vao, i, i);
    glError = glGetError();
    if (glError != GL_NO_ERROR) {
      glDeleteVertexArrays(1, &vao);
      return std::unexpected(errorGL("glVertexArrayAttribBinding", glError));
    }

    glEnableVertexArrayAttrib(vao, i);
    glError = glGetError();
    if (glError != GL_NO_ERROR) {
      glDeleteVertexArrays(1, &vao);
      return std::unexpected(errorGL("glEnableVertexArrayAttrib", glError));
    }
  }

  return VertexArrayObject(vao);
}

VertexArrayObject::VertexArrayObject(GLuint vao)
  :m_vao(vao)
{}

VertexArrayObject::VertexArrayObject(VertexArrayObject&& other) {
  m_vao = other.m_vao;
  other.m_vao = 0;
}

VertexArrayObject::~VertexArrayObject() {
  if (m_vao != 0) {
    glDeleteVertexArrays(1, &m_vao);
  }
}


auto VertexArrayObject::operator=(VertexArrayObject&& other) -> VertexArrayObject& {
  m_vao = other.m_vao;
  other.m_vao = 0;

  return *this;
}

auto VertexArrayObject::bind() -> void {
  glBindVertexArray(m_vao);
}
auto VertexArrayObject::unbind() -> void {
  glBindVertexArray(0);
}

auto VertexArrayObject::bindBuffer(
  GLuint idx,
  engine::VertexBuffer& buffer,
  GLintptr offset,
  GLsizei stride
) -> std::expected<void, engine::Error> {
  glVertexArrayVertexBuffer(m_vao, idx, *buffer, offset, stride);
  const GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    return std::unexpected(errorGL("glVertexArrayVertexBuffer", error));
  }

  return {};
}

}