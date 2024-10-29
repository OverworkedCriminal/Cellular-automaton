#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/Shader.hpp"
#include <format>

namespace engine {

auto Program::create(
  const std::vector<Shader*>& shaders
) -> std::expected<Program, std::string> {
  const GLuint program = glCreateProgram();
  if (program == 0) {
    const GLenum error = glGetError();
    return std::unexpected(std::format("glCreateProgram error: 0x{:04x}", error));
  }

  for (auto shader : shaders) {
    glAttachShader(program, **shader);
    const GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
      return std::unexpected(std::format("glAttachShader error: 0x{:04x}", error));
    }
  }

  glLinkProgram(program);

  GLint linkStatus;
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
  if (linkStatus != GL_TRUE) {
    GLint infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::string infoLog;
    infoLog.resize(infoLogLength);
    glGetProgramInfoLog(program, infoLog.size(), &infoLogLength, infoLog.data());

    return std::unexpected("failed to link program: " + infoLog);
  }

  Program outProgram(program);

  return outProgram;  
}

Program::Program(GLuint program)
  :m_program(program)
{

}

Program::Program(Program&& other) {
  m_program = other.m_program;
  other.m_program = 0;
}

Program::~Program() {
  if (m_program != 0) {
    glDeleteProgram(m_program);
  }
}

auto Program::operator=(Program&& other) -> Program& {
  m_program = other.m_program;
  other.m_program = 0;
  return *this;
}

auto Program::useProgram() -> std::expected<void, std::string> {
  glUseProgram(m_program);
  const GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    return std::unexpected(std::format("glUseProgram failed: 0x{:04x}", error));
  }

  return {};
}

}