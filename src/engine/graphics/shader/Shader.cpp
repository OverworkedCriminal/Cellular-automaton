#include "engine/graphics/shader/Shader.hpp"
#include <format>

namespace engine {

auto Shader::create(
  GLenum shaderType,
  std::string sourceCode
) -> std::expected<Shader, std::string> {
  const GLuint shader = glCreateShader(shaderType);
  if (shader == 0) {
    const GLenum error = glGetError();
    return std::unexpected(std::format("glCreateShader error: {}", error));
  }

  const char* sourceCodeCString = sourceCode.c_str();

  glShaderSource(shader, 1, &sourceCodeCString, NULL);
  glCompileShader(shader);

  GLint compileStatus;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus != GL_TRUE) {
    GLint infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::string infoLog;
    infoLog.resize(infoLogLength);
    glGetShaderInfoLog(shader, infoLog.size(), &infoLogLength, infoLog.data());

    return std::unexpected("failed to compile shader: " + infoLog);
  }

  Shader outShader(shader);

  return outShader;
}

Shader::Shader(GLuint shader)
  :m_shader(shader)
{

}

Shader::Shader(Shader&& other) {
  m_shader = other.m_shader;
  other.m_shader = 0;
}

Shader::~Shader() {
  if (m_shader != 0) {
    glDeleteShader(m_shader);
  }
}

auto Shader::operator=(Shader&& other) -> Shader& {
  m_shader = other.m_shader;
  other.m_shader = 0;
  return *this;
}

}