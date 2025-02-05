#ifndef ENGINE_GRAPHICS_TEXTURE_TEXTURE_HPP
#define ENGINE_GRAPHICS_TEXTURE_TEXTURE_HPP

#include "engine/error/Error.hpp"
#include "glad/glad.h"
#include <expected>
#include <vector>

namespace engine {

class Texture {
public:
  static auto create(
    GLsizei width,
    GLsizei height
  ) -> std::expected<Texture, Error>;

  Texture(const Texture&) = delete;
  Texture(Texture&&);
  ~Texture();

  auto operator=(const Texture&) -> Texture& = delete;
  auto operator=(Texture&&) -> Texture&;

  auto bind(GLint unit) -> void;
  auto bindImageTexture(GLint unit) -> std::expected<void, Error>;

  auto store(const std::vector<GLfloat>& buffer) -> std::expected<void, Error>;

private:
  Texture(GLuint texture, GLsizei width, GLsizei height);

  GLuint m_texture;

  GLsizei m_width;
  GLsizei m_height;
};

}


#endif