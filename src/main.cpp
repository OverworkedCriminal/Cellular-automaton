#include "engine/Config.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/engine.hpp"
#include <iostream>
#include <memory>

int main() {
  const engine::Config config = {
    .windowTitle = "Cellular automaton",
    .windowWidth = 800,
    .windowHeight = 600
  };

  auto application = std::make_unique<engine::IApplication>();

  auto result = engine::run(config, std::move(application));
  if (!result.has_value()) {
    std::cerr << "Engine failed: " << result.error() << '\n';
  }

  return 0;
}