#ifndef ENGINE_GRAPHICS_SHADER_SHADER_HPP
#define ENGINE_GRAPHICS_SHADER_SHADER_HPP

#include "glad/glad.h"
#include <expected>
#include <string>

namespace engine {

class Shader {
public:
  static auto create(
    GLenum shaderType,
    std::string sourceCode
  ) -> std::expected<Shader, std::string>;

  Shader(const Shader&) = delete;
  Shader(Shader&&);
  ~Shader();

  auto operator=(const Shader&) -> Shader& = delete;
  auto operator=(Shader&&) -> Shader&;

  auto operator*() -> GLuint;

private:
  Shader(GLuint shader);

  GLuint m_shader;
};

}

#endif