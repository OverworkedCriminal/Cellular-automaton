#ifndef ENGINE_GRAPHICS_BUFFER_VERTEX_BUFFER_HPP
#define ENGINE_GRAPHICS_BUFFER_VERTEX_BUFFER_HPP

#include "glad/glad.h"
#include <array>
#include <expected>
#include <string>

namespace engine {

class VertexBuffer {
public:
  static auto create(
    const std::array<GLfloat, 16>& data
  ) -> std::expected<VertexBuffer, std::string>;

  VertexBuffer(const VertexBuffer&) = delete;
  VertexBuffer(VertexBuffer&&);
  ~VertexBuffer();

  auto operator=(const VertexBuffer&) -> VertexBuffer& = delete;
  auto operator=(VertexBuffer&&) -> VertexBuffer&;

  auto bind() -> void;

private:
  VertexBuffer(GLuint buffer);

  GLuint m_buffer;
};

}

#endif