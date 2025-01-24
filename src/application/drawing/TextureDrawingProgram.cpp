#include "application/drawing/TextureDrawingProgram.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/buffer/VertexArray.hpp"
#include "engine/graphics/buffer/VertexBuffer.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/Shader.hpp"
#include "engine/utils/error.hpp"
#include <vector>

using engine::Error;
using engine::error;
using engine::errorGL;
using engine::VertexArray;
using engine::VaoAttribute;
using engine::VertexBuffer;
using engine::Program;
using engine::Texture;
using engine::Shader;

constexpr int DRAWING_PROGRAM_TEXTURE_UNIT = 0;

auto createVao() -> std::expected<VertexArray, Error> {
  const std::vector<VaoAttribute> vaoAttributes = {
    {
      .size = 2,
      .type = GL_FLOAT,
      .normalized = GL_FALSE
    },
    {
      .size = 2,
      .type = GL_FLOAT,
      .normalized = GL_FALSE
    }
  };

  return VertexArray::create(vaoAttributes);
}

auto createVbo() -> std::expected<VertexBuffer, Error> {
  const std::vector<GLfloat> vboData = {
    //   POS        TEX_POS
    -1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f
  };

  return VertexBuffer::create(vboData);
}

auto bindVbo(
  VertexArray& vao,
  VertexBuffer& vbo
) -> std::expected<void, Error> {
  std::expected<void, Error> bindResult;
  bindResult = vao.bindBuffer(0, vbo, 0 * sizeof(GLfloat), 4 * sizeof(GLfloat));
  if (!bindResult.has_value()) {
    return std::unexpected(error("failed to bind position data", bindResult.error()));
  }
  bindResult = vao.bindBuffer(1, vbo, 2 * sizeof(GLfloat), 4 * sizeof(GLfloat));
  if (!bindResult.has_value()) {
    return std::unexpected(error("failed to bind texture data", bindResult.error()));
  }

  return {};
}

auto createProgram() -> std::expected<Program, Error> {
  auto vertexShader = Shader::create_from_file(GL_VERTEX_SHADER, "shaders/texture.vertex.glsl");
  if (!vertexShader.has_value()) {
    return std::unexpected(error("failed to create vertex shader", vertexShader.error()));
  }

  auto fragmentShader = Shader::create_from_file(GL_FRAGMENT_SHADER, "shaders/texture.fragment.glsl");
  if (!fragmentShader.has_value()) {
    return std::unexpected(error("failed to create fragment shader", fragmentShader.error()));
  }

  const std::vector<Shader*> shaders = { &*vertexShader, &*fragmentShader };
  auto program = Program::create(shaders);
  if (!program.has_value()) {
    return std::unexpected(error("failed to create program", program.error()));
  }

  auto useProgramResult = program->useProgram();
  if (!useProgramResult.has_value()) {
    return std::unexpected(error("failed to use program", useProgramResult.error()));
  }

  auto uniformResult = program->setUniform("simulationTexture", DRAWING_PROGRAM_TEXTURE_UNIT);
  if (!uniformResult.has_value()) {
    return std::unexpected(error("failed to set simulationTexture uniform", uniformResult.error()));
  }

  return std::move(*program);
}

auto TextureDrawingProgram::create() -> std::expected<TextureDrawingProgram, Error> {
  auto vbo = createVbo();
  if (!vbo.has_value()) {
    return std::unexpected(error("failed to create vbo", vbo.error()));
  }

  auto vao = createVao();
  if (!vao.has_value()) {
    return std::unexpected(error("failed to create vao", vao.error()));
  }

  auto bindResult = bindVbo(*vao, *vbo);
  if (!bindResult.has_value()) {
    return std::unexpected(error("failed to attach vbo to vao", bindResult.error()));
  }

  auto program = createProgram();
  if (!program.has_value()) {
    return std::unexpected(error("failed to create program", program.error()));
  }

  return TextureDrawingProgram(
    std::move(*vao),
    std::move(*vbo),
    std::move(*program)
  );
}

TextureDrawingProgram::TextureDrawingProgram(
  VertexArray vao,
  VertexBuffer vbo,
  Program program
)
  :m_vao(std::move(vao))
  ,m_vbo(std::move(vbo))
  ,m_program(std::move(program))
{}

auto TextureDrawingProgram::draw(Texture& texture) -> std::expected<void, Error> {
  auto useProgramResult = m_program.useProgram();
  if (!useProgramResult.has_value()) {
    return std::unexpected(error("failed to use program", useProgramResult.error()));
  }

  m_vao.bind();
  texture.bind(DRAWING_PROGRAM_TEXTURE_UNIT);

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  const GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    m_vao.unbind();
    return std::unexpected(errorGL("glDrawArrays", error));
  }

  m_vao.unbind();

  return {};
}