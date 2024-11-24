#ifndef APPLICATION_DRAWING_TEXTURE_DRAWING_PROGRAM_HPP
#define APPLICATION_DRAWING_TEXTURE_DRAWING_PROGRAM_HPP

#include "engine/graphics/buffer/VertexArray.hpp"
#include "engine/graphics/buffer/VertexBuffer.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include <expected>

/**
 * @brief
 * Drawing program that allows to draw a texture over whole screen
 */
class TextureDrawingProgram {
public:
  static auto create() -> std::expected<TextureDrawingProgram, engine::Error>;

  TextureDrawingProgram(const TextureDrawingProgram&) = delete;
  TextureDrawingProgram(TextureDrawingProgram&&) = default;

  auto operator=(const TextureDrawingProgram&) -> TextureDrawingProgram& = delete;
  auto operator=(TextureDrawingProgram&&) -> TextureDrawingProgram& = default;

  auto draw(engine::Texture& texture) -> std::expected<void, engine::Error>;

private:
  TextureDrawingProgram(
    engine::VertexArray m_vao,
    engine::VertexBuffer m_vbo,
    engine::Program m_program
  );

  engine::VertexArray m_vao;
  engine::VertexBuffer m_vbo;
  engine::Program m_program;
};

#endif