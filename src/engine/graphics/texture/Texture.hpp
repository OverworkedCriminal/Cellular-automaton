#ifndef ENGINE_GRAPHICS_TEXTURE_TEXTURE_HPP
#define ENGINE_GRAPHICS_TEXTURE_TEXTURE_HPP

#include "engine/error/Error.hpp"
#include "glad/glad.h"
#include <expected>

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

  auto bind() -> void;

private:
  Texture(GLuint texture);

  GLuint m_texture;
};

}


#endif