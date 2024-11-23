#ifndef ENGINE_GPRAHICS_BUFFER_VERTEX_ARRAY_OBJECT_HPP
#define ENGINE_GPRAHICS_BUFFER_VERTEX_ARRAY_OBJECT_HPP

#include "engine/error/Error.hpp"
#include "engine/graphics/buffer/VertexBuffer.hpp"
#include "glad/glad.h"
#include <expected>
#include <vector>

namespace engine {

struct VaoAttribute {
  /**
   * Must be 1, 2, 3, 4
   */
  GLint size;
  GLenum type;
  GLboolean normalized;
};

class VertexArrayObject {
public:
  static auto create(
    const std::vector<VaoAttribute>& attributes
  ) -> std::expected<VertexArrayObject, Error>;

  VertexArrayObject(const VertexArrayObject&) = delete;
  VertexArrayObject(VertexArrayObject&&);
  ~VertexArrayObject();

  auto operator=(const VertexArrayObject&) -> VertexArrayObject& = delete;
  auto operator=(VertexArrayObject&&) -> VertexArrayObject&;

  auto bind() -> void;
  auto bindBuffer(
    GLuint idx,
    engine::VertexBuffer& buffer,
    GLintptr offset,
    GLsizei stride
  ) -> std::expected<void, engine::Error>;

private:
  VertexArrayObject(GLuint vao);

  GLuint m_vao;
};

}


#endif