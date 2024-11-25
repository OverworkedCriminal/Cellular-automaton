#include "application/simulation/gpu/GpuSimulation.hpp"
#include "engine/Config.hpp"
#include "engine/engine.hpp"
#include <iostream>
#include <memory>

int main() {
  constexpr int WINDOW_WIDTH = 800;
  constexpr int WINDOW_HEIGHT = 600;

  constexpr int SIMULATION_WIDTH = 100;
  constexpr int SIMULATION_HEIGHT = 100;

  const engine::Config config = {
    .windowTitle = "Cellular automaton",
    .windowWidth = WINDOW_WIDTH,
    .windowHeight = WINDOW_HEIGHT
  };

  auto simulation = GpuSimulation::create(SIMULATION_WIDTH, SIMULATION_HEIGHT);
  if (!simulation.has_value()) {
    std::cerr << "Simulation creation failed\n\t" << simulation.error() << '\n';
    return -1;
  }
  auto simulationPtr = std::make_unique<GpuSimulation>(std::move(*simulation));

  auto result = engine::run(config, std::move(simulationPtr));
  if (!result.has_value()) {
    std::cerr << "Engine failed:\n\t" << result.error() << '\n';
    return -1;
  }

  return 0;
}