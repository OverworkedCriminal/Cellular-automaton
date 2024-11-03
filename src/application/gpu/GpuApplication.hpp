#ifndef APPLICATION_GPU_GPU_APPLICATION_HPP
#define APPLICATION_GPU_GPU_APPLICATION_HPP

#include "engine/application/IApplication.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/buffer/VertexArrayObject.hpp"
#include "engine/graphics/buffer/VertexBuffer.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include <optional>

class GpuApplication :public engine::IApplication {
public:
  static auto create(int width, int height) -> std::expected<GpuApplication, engine::Error>;
  
  GpuApplication(const GpuApplication&) = delete;
  GpuApplication(GpuApplication&&);

  auto operator=(const GpuApplication&) -> GpuApplication& = delete;
  auto operator=(GpuApplication&&) -> GpuApplication&;

  auto onCreate(
    const engine::Context& context
  ) -> std::expected<void, engine::Error> override;

  auto onDestroy(
    const engine::Context& context
  ) -> std::expected<void, engine::Error> override;

  auto onUpdate(
    const engine::Context& context
  ) -> std::expected<void, engine::Error> override;

private:
  GpuApplication(int width, int height);

  auto initDrawing() -> std::expected<void, engine::Error>;
  auto initSimulation() -> std::expected<void, engine::Error>;

  int m_width;
  int m_height;

  int m_computeSpaceX;
  int m_computeSpaceY;

  std::optional<engine::VertexArrayObject> m_vao;
  std::optional<engine::VertexBuffer> m_vertexBuffer;

  std::optional<engine::Texture> m_texture;
  std::optional<engine::ShaderStorageBuffer> m_inputSSBO;
  std::optional<engine::ShaderStorageBuffer> m_outputSSBO;

  std::optional<engine::Program> m_simulationProgram;
  std::optional<engine::Program> m_drawingProgram;


};

#endif