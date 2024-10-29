#ifndef ENGINE_GRAPHICS_SHADER_PROGRAM_HPP
#define ENGINE_GRAPHICS_SHADER_PROGRAM_HPP

#include "engine/graphics/shader/Shader.hpp"
#include "glad/glad.h"
#include <expected>
#include <string>
#include <vector>

namespace engine {

class Program {
public:
  static auto create(
    const std::vector<Shader*>& shaders
  ) -> std::expected<Program, std::string>;

  Program(const Program&) = delete;
  Program(Program&&);
  ~Program();

  auto operator=(const Program&) -> Program& = delete;
  auto operator=(Program&&) -> Program&;

  auto useProgram() -> std::expected<void, std::string>;

private:
  Program(GLuint program);

  GLuint m_program;

};

}


#endif