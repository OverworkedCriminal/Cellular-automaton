#ifndef APPLICATION_SIMULATION_GPU_GPU_SIMULATION_HPP
#define APPLICATION_SIMULATION_GPU_GPU_SIMULATION_HPP

#include "application/simulation/ISimulation.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include <expected>

class GpuSimulation :public ISimulation {
public:
  static auto create(
    int width,
    int height
  ) -> std::expected<GpuSimulation, engine::Error>;

  GpuSimulation(const GpuSimulation&) = delete;
  GpuSimulation(GpuSimulation&&) = default;

  auto operator=(const GpuSimulation&) -> GpuSimulation& = delete;
  auto operator=(GpuSimulation&&) -> GpuSimulation& = default;

  auto width() -> unsigned int override;
  auto height() -> unsigned int override;
  auto padding() -> unsigned int override;

  auto paint(const std::function<void(SimulationGrid&)>& paintFn) -> void override;
  auto onCreate(const Context& applicationContext) -> std::expected<void, engine::Error> override;
  auto onUpdate(const Context& applicationContext) -> std::expected<void, engine::Error> override;

private:
  GpuSimulation(int width, int height);

  auto initSimulationGrid(bool isGpuBigEndian) -> std::expected<void, engine::Error>;
  auto initSimulation() -> std::expected<void, engine::Error>;

  unsigned int m_width;
  unsigned int m_height;

  int m_computeSpaceX;
  int m_computeSpaceY;

  std::optional<std::vector<std::byte>> m_buffer;
  std::optional<SimulationGrid> m_simulationGrid;

  std::optional<engine::Program> m_simulationProgram;
  std::optional<engine::ShaderStorageBuffer> m_inputSSBO;
  std::optional<engine::ShaderStorageBuffer> m_outputSSBO;
};

#endif