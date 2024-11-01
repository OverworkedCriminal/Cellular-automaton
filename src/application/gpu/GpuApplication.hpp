#ifndef APPLICATION_GPU_GPU_APPLICATION_HPP
#define APPLICATION_GPU_GPU_APPLICATION_HPP

#include "engine/application/IApplication.hpp"
#include "engine/graphics/buffer/VertexArrayObject.hpp"
#include "engine/graphics/buffer/VertexBuffer.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include <optional>

class GpuApplication :public engine::IApplication {
public:
  static auto create(int width, int height) -> std::expected<GpuApplication, std::string>;
  
  GpuApplication(const GpuApplication&) = delete;
  GpuApplication(GpuApplication&&);

  auto operator=(const GpuApplication&) -> GpuApplication& = delete;
  auto operator=(GpuApplication&&) -> GpuApplication&;

  auto onCreate(
    const Context& context
  ) -> std::expected<void, std::string> override;

  auto onDestroy(
    const Context& context
  ) -> std::expected<void, std::string> override;

  auto onUpdate(
    const Context& context
  ) -> std::expected<void, std::string> override;

private:
  GpuApplication(int width, int height);

  auto initDrawing() -> std::expected<void, std::string>;
  auto initSimulation() -> std::expected<void, std::string>;

  int m_width;
  int m_height;

  std::optional<engine::VertexArrayObject> m_vao;
  std::optional<engine::VertexBuffer> m_vertexBuffer;

  std::optional<engine::Texture> m_texture;

  std::optional<engine::Program> m_simulationProgram;
  std::optional<engine::Program> m_drawingProgram;

};

#endif