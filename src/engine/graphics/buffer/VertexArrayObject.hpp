#ifndef ENGINE_GPRAHICS_BUFFER_VERTEX_ARRAY_OBJECT_HPP
#define ENGINE_GPRAHICS_BUFFER_VERTEX_ARRAY_OBJECT_HPP

#include "glad/glad.h"

namespace engine {

class VertexArrayObject {
public:
  static auto create() -> VertexArrayObject;

  VertexArrayObject(const VertexArrayObject&) = delete;
  VertexArrayObject(VertexArrayObject&&);
  ~VertexArrayObject();

  auto operator=(const VertexArrayObject&) -> VertexArrayObject& = delete;
  auto operator=(VertexArrayObject&&) -> VertexArrayObject&;

  auto bind() -> void;

private:
  VertexArrayObject(GLuint vao);

  GLuint m_vao;
};

}


#endif