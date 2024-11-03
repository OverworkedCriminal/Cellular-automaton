#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/Shader.hpp"
#include "engine/utils/error.hpp"

namespace engine {

auto Program::create(
  const std::vector<Shader*>& shaders
) -> std::expected<Program, Error> {
  const GLuint program = glCreateProgram();
  if (program == 0) {
    const GLenum glError = glGetError();
    return std::unexpected(errorGL("glCreateProgram", glError));
  }

  for (auto shader : shaders) {
    glAttachShader(program, **shader);
    const GLenum glError = glGetError();
    if (glError != GL_NO_ERROR) {
      glDeleteProgram(program);
      return std::unexpected(errorGL("glAttachShader", glError));
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

    glDeleteProgram(program);
    return std::unexpected(error("failed to link program: " + infoLog));
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

auto Program::useProgram() -> std::expected<void, Error> {
  glUseProgram(m_program);
  const GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glUseProgram", glError));
  }

  return {};
}

auto Program::setUniform(
  const std::string& name,
  GLuint value
) -> std::expected<void, Error> {
  auto locationResult = getUniformLocation(name);
  if (!locationResult.has_value()) {
    return std::unexpected(std::move(locationResult.error()));
  }

  glUniform1ui(*locationResult, value);
  const GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glUniform1ui", glError));
  }

  return {};
}

auto Program::setUniform(
  const std::string& name,
  GLint value
) -> std::expected<void, Error> {
  auto locationResult = getUniformLocation(name);
  if (!locationResult.has_value()) {
    return std::unexpected(std::move(locationResult.error()));
  }

  glUniform1i(*locationResult, value);
  const GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glUniform1ui", glError));
  }

  return {};
}

auto Program::getUniformLocation(const std::string& name) -> std::expected<GLint, Error> {
  const GLint location = glGetUniformLocation(m_program, name.c_str());
  const GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    return std::unexpected(errorGL("glGetUniformLocation", glError));
  }

  if (location == -1) {
    return std::unexpected(error("failed to find uniform location of " + name));
  }

  return location;
}

}