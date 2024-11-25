#ifndef APPLICATION_SIMULATION_GPU_GPU_SIMULATION_HPP
#define APPLICATION_SIMULATION_GPU_GPU_SIMULATION_HPP

#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/simulation/Cell.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include <expected>

class GpuSimulation :public engine::IApplication {
public:
  static auto create(
    int width,
    int height
  ) -> std::expected<GpuSimulation, engine::Error>;

  GpuSimulation(const GpuSimulation&) = delete;
  GpuSimulation(GpuSimulation&&) = default;

  auto operator=(const GpuSimulation&) -> GpuSimulation& = delete;
  auto operator=(GpuSimulation&&) -> GpuSimulation& = default;

  auto onCreate(const engine::Context& applicationContext) -> std::expected<void, engine::Error> override;
  auto onUpdate(const engine::Context& applicationContext) -> std::expected<void, engine::Error> override;

private:
  GpuSimulation(int width, int height);

  auto initSimulation() -> std::expected<void, engine::Error>;
  auto initDrawing() -> std::expected<void, engine::Error>;

  auto paint(Cell cell, const engine::Context& applicationContext) -> void;

  unsigned int m_width;
  unsigned int m_height;

  std::optional<std::vector<Cell>> m_buffer;
  unsigned int m_bufferValueOffset;
  unsigned int m_bufferValueStride;

  std::optional<engine::Program> m_simulationProgram;
  std::optional<engine::ShaderStorageBuffer> m_inputSSBO;
  std::optional<engine::ShaderStorageBuffer> m_outputSSBO;
  int m_computeSpaceX;
  int m_computeSpaceY;

  std::optional<engine::Texture> m_drawingTexture;
  std::optional<TextureDrawingProgram> m_drawingProgram;
};

#endif