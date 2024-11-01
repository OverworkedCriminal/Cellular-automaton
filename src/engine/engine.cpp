#include "engine/Config.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/window/Window.hpp"

namespace engine {

static auto validate_config(const Config& config) -> std::expected<void, std::string> {
  if (config.windowWidth <= 0) {
    return std::unexpected("windowWidth must be positive");
  } else if (config.windowHeight <= 0) {
    return std::unexpected("windowHeight must be positive");
  } else {
    return {};
  }
}

auto run(
  const Config& config,
  IApplication& application
) -> std::expected<void, std::string> {
  auto validationResult = validate_config(config);
  if (!validationResult.has_value()) {
    return std::unexpected("invalid config: " + validationResult.error());
  }

  auto windowResult = Window::open(
    config.windowTitle,
    config.windowWidth,
    config.windowHeight
  );
  if (!windowResult.has_value()) {
    return std::unexpected("failed to open window: " + windowResult.error());
  }
  Window& window = *windowResult;

  auto onCreateResult = application.onCreate();
  if (!onCreateResult.has_value()) {
    return std::unexpected("application onCreate failed: " + onCreateResult.error());
  }

  while (!glfwWindowShouldClose(*window)) {
    auto onUpdateResult = application.onUpdate();
    if (!onUpdateResult.has_value()) {
      return std::unexpected("application onUpdate failed " + onUpdateResult.error());
    }

    glfwSwapBuffers(*window);
    glfwPollEvents();
  }

  auto onDestroyResult = application.onDestroy();
  if (!onDestroyResult.has_value()) {
    return std::unexpected("application onDestroy failed: " + onDestroyResult.error());
  }

  return {};
}

}
