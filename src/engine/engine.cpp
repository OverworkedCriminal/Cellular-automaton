#include "engine/engine.hpp"
#include "engine/Config.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/window/Window.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace engine {

class Engine {
public:
  static auto create(Window window) -> Engine {
    int width, height;
    glfwGetWindowSize(*window, &width, &height);

    return Engine(std::move(window), width, height);
  }

  Engine(const Engine&) = delete;
  Engine(Engine&& other)
    :m_window(std::move(other.m_window))
    ,m_windowWidth(other.m_windowWidth)
    ,m_windowHeight(other.m_windowHeight)
  {}

  auto operator=(const Engine&) -> Engine& = delete;
  auto operator=(Engine&& other) -> Engine& {
    m_window = std::move(other.m_window);
    m_windowWidth = other.m_windowWidth;
    m_windowHeight = other.m_windowHeight;
    return *this;
  }

  auto run(std::unique_ptr<IApplication> application) -> std::expected<void, std::string> {
    updateContext();

    auto onCreateResult = application->onCreate(m_context);
    if (!onCreateResult.has_value()) {
      return std::unexpected("application onCreate failed: " + onCreateResult.error());
    }

    while (!glfwWindowShouldClose(*m_window)) {
      auto onUpdateResult = application->onUpdate(m_context);
      if (!onUpdateResult.has_value()) {
        std::cerr << "application onUpdate failed. Main loop stopped " << onUpdateResult.error() << '\n';
        break;
      }

      glfwSwapBuffers(*m_window);
      glfwPollEvents();
      updateContext();
    }

    auto onDestroyResult = application->onDestroy(m_context);
    if (!onDestroyResult.has_value()) {
      return std::unexpected("application onDestroy failed: " + onDestroyResult.error());
    }

    return {};
  }

private:
  Engine(Window window, int windowWidth, int windowHeight) 
    :m_window(std::move(window))
    ,m_windowWidth(windowWidth)
    ,m_windowHeight(windowHeight)
  {}

  auto updateContext() -> void {
    double posX, posY;
    glfwGetCursorPos(*m_window, &posX, &posY);

    int leftState = glfwGetMouseButton(*m_window, GLFW_MOUSE_BUTTON_LEFT);

    m_context.mousePosX = std::clamp(static_cast<int>(std::floor(posX)), 0, m_windowWidth);
    m_context.mousePosY = std::clamp(static_cast<int>(std::floor(posY)), 0, m_windowHeight);
    m_context.mousePressed = leftState == GLFW_PRESS;
  }

  Window m_window;

  Context m_context;

  int m_windowWidth;
  int m_windowHeight;

};

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
  std::unique_ptr<IApplication> application
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

  Engine engine = Engine::create(std::move(*windowResult));
  auto runResult = engine.run(std::move(application));
  if (!runResult.has_value()) {
    return std::unexpected("run failed: " + runResult.error());
  }

  return {};
}

}
