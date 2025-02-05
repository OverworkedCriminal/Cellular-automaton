#ifndef ENGINE_GRAPHICS_SHADER_SHADER_STORAGE_BUFFER_HPP
#define ENGINE_GRAPHICS_SHADER_SHADER_STORAGE_BUFFER_HPP

#include "engine/error/Error.hpp"
#include "glad/glad.h"
#include <expected>
#include <vector>

namespace engine {

class ShaderStorageBuffer {
public:
  static auto create(GLsizeiptr size) -> std::expected<ShaderStorageBuffer, Error>;

  ShaderStorageBuffer(const ShaderStorageBuffer&) = delete;
  ShaderStorageBuffer(ShaderStorageBuffer&&);
  ~ShaderStorageBuffer();

  auto operator=(const ShaderStorageBuffer&) -> ShaderStorageBuffer& = delete;
  auto operator=(ShaderStorageBuffer&&) -> ShaderStorageBuffer&;

  auto bindBufferBase(GLuint index) -> std::expected<void, Error>;

  template<typename T>
  auto store(const std::vector<T>& buffer) -> void {
    const GLsizeiptr bufferByteSize = buffer.size() * sizeof(T);
    const GLsizeiptr size = std::min(m_size, bufferByteSize);
    glNamedBufferSubData(m_ssbo, 0, size, buffer.data());
  }

  template<typename T>
  auto load(std::vector<T>& buffer) -> void {
    const GLsizeiptr bufferByteSize = buffer.size() * sizeof(T);
    const GLsizeiptr size = std::min(m_size, bufferByteSize);
    glGetNamedBufferSubData(m_ssbo, 0, size, buffer.data());
  }

private:
  ShaderStorageBuffer(GLuint ssbo, GLsizeiptr size);

  GLuint m_ssbo;
  GLsizeiptr m_size;
};

}

#endif