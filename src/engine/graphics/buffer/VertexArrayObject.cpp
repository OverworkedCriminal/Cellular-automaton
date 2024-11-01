#include "engine/graphics/buffer/VertexArrayObject.hpp"

namespace engine {

auto VertexArrayObject::create() -> VertexArrayObject {
  GLuint vao;
  glGenVertexArrays(1, &vao);

  VertexArrayObject outVao(vao);

  return outVao;
}

VertexArrayObject::VertexArrayObject(GLuint vao)
  :m_vao(vao)
{

}

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

}