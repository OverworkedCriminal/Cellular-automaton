#include "engine/graphics/texture/Texture.hpp"
#include "engine/utils/error.hpp"
#include <cassert>
#include <vector>

namespace engine {

auto Texture::create(
  GLsizei width,
  GLsizei height
) -> std::expected<Texture, Error> {
  if (width <= 0 || height <= 0) {
    return std::unexpected(error("invalid texture dimensions"));
  }

  // times 4 because of RGBA channels
  const std::vector<GLfloat> initialTextureState(width * height * 4, 0.0f);

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
    GL_RGBA32F,
    width,
    height,
    0,
    GL_RGBA,
    GL_FLOAT,
    initialTextureState.data()
  );
  const GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glTexImage2D", glError));
  }

  return Texture(texture, width, height);
}

Texture::Texture(GLuint texture, GLsizei width, GLsizei height)
  :m_texture(texture)
  ,m_width(width)
  ,m_height(height)
{}

Texture::Texture(Texture&& other) {
  m_texture = other.m_texture;
  m_width = other.m_width;
  m_height = other.m_height;
  other.m_texture = 0;
}

Texture::~Texture() {
  if (m_texture != 0) {
    glDeleteTextures(1, &m_texture);
  }
}

auto Texture::operator=(Texture&& other) -> Texture& {
  m_texture = other.m_texture;
  m_width = other.m_width;
  m_height = other.m_height;
  other.m_texture = 0;
  return *this;
}

auto Texture::bind(GLint unit) -> void {
  assert(unit >= 0 && unit < 32);

  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, m_texture);
}

auto Texture::bindImageTexture(GLint unit) -> std::expected<void, Error> {
  assert(unit >= 0 && unit < 32);

  glBindImageTexture(
    unit,
    m_texture,
    0,
    GL_FALSE,
    0,
    GL_READ_WRITE,
    GL_RGBA32F
  );
  const GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glBindImageTexture", glError));
  }

  return {};
}

auto Texture::store(const std::vector<GLfloat>& buffer) -> std::expected<void, Error> {
  glTextureSubImage2D(
    m_texture,
    0,
    0,
    0,
    m_width,
    m_height,
    GL_RGBA,
    GL_FLOAT,
    buffer.data()
  );
  const GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glTextureSubImage2D", glError));
  }

  return {};
}

}
