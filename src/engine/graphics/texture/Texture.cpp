#include "engine/graphics/texture/Texture.hpp"
#include <format>
#include <vector>

namespace engine {

auto Texture::create(
  GLsizei width,
  GLsizei height
) -> std::expected<Texture, std::string> {
  if (width <= 0 || height <= 0) {
    return std::unexpected("invalid texture dimensions");
  }

  // times 3 because of RGB channels
  const std::vector<GLfloat> initialTextureState(width * height * 3, 0.5f);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  const std::array<GLfloat, 4> borderColor = { 0.0f, 0.0f, 0.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGB,
    width,
    height,
    0,
    GL_RGB,
    GL_FLOAT,
    initialTextureState.data()
  );

  const GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    return std::unexpected(std::format("glTexImage2D failed: 0x{:04x}", error));
  }

  Texture outTexture(texture);

  return outTexture;
}

Texture::Texture(GLuint texture)
  :m_texture(texture)
{

}

Texture::Texture(Texture&& other) {
  m_texture = other.m_texture;
  other.m_texture = 0;
}

Texture::~Texture() {
  if (m_texture != 0) {
    glDeleteTextures(1, &m_texture);
  }
}

auto Texture::operator=(Texture&& other) -> Texture& {
  m_texture = other.m_texture;
  other.m_texture = 0;
  return *this;
}

auto Texture::bind() -> void {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture);
}

}
