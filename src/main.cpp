#include "application/gpu/GpuApplication.hpp"
#include "engine/Config.hpp"
#include "engine/engine.hpp"
#include <iostream>

int main() {
  constexpr int WINDOW_WIDTH = 800;
  constexpr int WINDOW_HEIGHT = 600;

  const engine::Config config = {
    .windowTitle = "Cellular automaton",
    .windowWidth = WINDOW_WIDTH,
    .windowHeight = WINDOW_HEIGHT
  };

  auto applicationResult = GpuApplication::create(WINDOW_WIDTH, WINDOW_HEIGHT);
  if (!applicationResult.has_value()) {
    std::cerr << "Failed to create GpuApplication " << applicationResult.error();
    return -1;
  }

  auto application = std::make_unique<GpuApplication>(std::move(*applicationResult));

  auto result = engine::run(config, std::move(application));
  if (!result.has_value()) {
    std::cerr << "Engine failed: " << result.error() << '\n';
    return -1;
  }

  return 0;
}