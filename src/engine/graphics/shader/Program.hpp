#ifndef ENGINE_GRAPHICS_SHADER_PROGRAM_HPP
#define ENGINE_GRAPHICS_SHADER_PROGRAM_HPP

#include "engine/graphics/shader/Shader.hpp"
#include "glad/glad.h"
#include <expected>
#include <vector>

namespace engine {

class Program {
public:
  static auto create(
    const std::vector<Shader*>& shaders
  ) -> std::expected<Program, Error>;

  Program(const Program&) = delete;
  Program(Program&&);
  ~Program();

  auto operator=(const Program&) -> Program& = delete;
  auto operator=(Program&&) -> Program&;

  auto useProgram() -> std::expected<void, Error>;

  auto setUniform(const std::string& name, GLuint value) -> std::expected<void, Error>;
  auto setUniform(const std::string& name, GLint value) -> std::expected<void, Error>;

private:
  Program(GLuint program);

  auto getUniformLocation(const std::string& name) -> std::expected<GLint, Error>;

  GLuint m_program;

};

}


#endif