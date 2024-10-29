#ifndef ENGINE_GRAPHICS_SHADER_SHADER_STORAGE_BUFFER_HPP
#define ENGINE_GRAPHICS_SHADER_SHADER_STORAGE_BUFFER_HPP

#include "glad/glad.h"
#include <expected>
#include <string>
#include <vector>

namespace engine {

class ShaderStorageBuffer {
public:
  static auto create(GLsizeiptr size) -> std::expected<ShaderStorageBuffer, std::string>;

  ShaderStorageBuffer(const ShaderStorageBuffer&) = delete;
  ShaderStorageBuffer(ShaderStorageBuffer&&);
  ~ShaderStorageBuffer();

  auto operator=(const ShaderStorageBuffer&) -> ShaderStorageBuffer& = delete;
  auto operator=(ShaderStorageBuffer&&) -> ShaderStorageBuffer&;

  auto bindBufferBase(GLuint index) -> std::expected<void, std::string>;

  auto store(const std::vector<GLubyte>& buffer) -> void;
  auto load(std::vector<GLubyte>& buffer) -> void;

private:
  ShaderStorageBuffer(GLuint ssbo, GLsizeiptr size);

  GLuint m_ssbo;
  GLsizeiptr m_size;
};

}

#endif