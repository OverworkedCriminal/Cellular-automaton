#ifndef ENGINE_GRAPHICS_TEXTURE_TEXTURE_HPP
#define ENGINE_GRAPHICS_TEXTURE_TEXTURE_HPP

#include "glad/glad.h"
#include <expected>
#include <string>

namespace engine {

class Texture {
public:
  static auto create(
    GLsizei width,
    GLsizei height
  ) -> std::expected<Texture, std::string>;

  Texture(const Texture&) = delete;
  Texture(Texture&&);
  ~Texture();

  auto operator=(const Texture&) -> Texture& = delete;
  auto operator=(Texture&&) -> Texture&;

private:
  Texture(GLuint texture);

  GLuint m_texture;
};

}


#endif