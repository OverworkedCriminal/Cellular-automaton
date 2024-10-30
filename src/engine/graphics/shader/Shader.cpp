#include "engine/graphics/shader/Shader.hpp"
#include "engine/utils/file.hpp"
#include <format>

namespace engine {

auto Shader::create(
  GLenum shaderType,
  const std::string& sourceCode
) -> std::expected<Shader, std::string> {
  const GLuint shader = glCreateShader(shaderType);
  if (shader == 0) {
    const GLenum error = glGetError();
    return std::unexpected(std::format("glCreateShader error: 0x{:04x}", error));
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

    glDeleteShader(shader);
    return std::unexpected("failed to compile shader: " + infoLog);
  }

  Shader outShader(shader);

  return outShader;
}

auto Shader::create_from_file(
  GLenum shaderType,
  const std::string &filepath
) -> std::expected<Shader, std::string> {
  auto readFileResult = read_file(filepath);
  if (!readFileResult.has_value()) {
    return std::unexpected("failed to read shader file: " + readFileResult.error());
  }

  auto shaderResult = create(shaderType, *readFileResult);
  if (!shaderResult.has_value()) {
    return std::unexpected("failed to create shader from file: " + shaderResult.error());
  }

  return std::move(*shaderResult);
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

auto Shader::operator*() -> GLuint {
  return m_shader;
}

}