#include "engine/engine.hpp"
#include "engine/Config.hpp"
#include "engine/GlfwWindowContext.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/input/InputSystem.hpp"
#include "engine/time/TimeSystem.hpp"
#include "engine/utils/defer.hpp"
#include "engine/utils/error.hpp"
#include "engine/window/WindowSystem.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

namespace engine {

using namespace input;
using namespace window;
using namespace time;

static auto keyboardCallback(
  GLFWwindow* window,
  int key,
  int scancode,
  int action,
  int mods
) -> void {
  void* userPointer = glfwGetWindowUserPointer(window);
  auto* context = reinterpret_cast<GlfwWindowContext*>(userPointer);
  context->inputSystem.keyboardCallback(window, key, scancode, action, mods);
}

static auto mousePositionCallback(
  GLFWwindow* window,
  double posX,
  double posY
) -> void {
  void* userPointer = glfwGetWindowUserPointer(window);
  auto* context = reinterpret_cast<GlfwWindowContext*>(userPointer);
  context->inputSystem.mousePositionCallback(window, posX, posY);
}

static auto mouseButtonCallback(
  GLFWwindow* window,
  int button,
  int action,
  int mods
) -> void {
  void* userPointer = glfwGetWindowUserPointer(window);
  auto* context = reinterpret_cast<GlfwWindowContext*>(userPointer);
  context->inputSystem.mouseButtonCallback(window, button, action, mods);
}

static auto framebufferSizeCallback(
  GLFWwindow* window,
  int width,
  int height
) -> void {
  void* userPointer = glfwGetWindowUserPointer(window);
  auto* context = reinterpret_cast<GlfwWindowContext*>(userPointer);
  context->windowSystem.framebufferSizeCallback(window, width, height);
}

static auto initGLFW(
  const std::string& windowTitle,
  int windowWidth,
  int windowHeight
) -> std::expected<GLFWwindow*, Error> {
  if (windowWidth <= 0 || windowHeight <= 0) {
    return std::unexpected(error("invalid dimensions"));
  }

  if (!glfwInit()) {
    return std::unexpected(error("failed to init GLFW"));
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(
    windowWidth,
    windowHeight,
    windowTitle.c_str(),
    nullptr,
    nullptr
  );
  if (!window) {
    glfwTerminate();
    return std::unexpected(error("failed to create window"));
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    glfwTerminate();
    return std::unexpected(error("failed to init GLAD"));
  }

  glfwSetKeyCallback(window, keyboardCallback);
  glfwSetCursorPosCallback(window, mousePositionCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  glViewport(0, 0, width, height);

  return window;
}

static auto destroyGLFW() -> void {
  glfwTerminate();
}

auto run(
  const Config& config,
  std::unique_ptr<IApplication> applicationPtr
) -> std::expected<void, Error> {
  auto window = initGLFW(config.windowTitle, config.windowWidth, config.windowHeight);
  if (!window.has_value()) {
    return std::unexpected(error("failed to init window", window.error()));
  }
  defer(destroyGLFW());
  defer(applicationPtr = nullptr);

  auto inputSystem = InputSystem::create(*window);
  auto windowSystem = WindowSystem::create(*window);
  auto timeSystem = TimeSystem::create();

  auto engineContext = EngineContext {
    .inputSystem = inputSystem,
    .windowSystem = windowSystem,
    .timeSystem = timeSystem
  };
  auto glfwWindowContext = GlfwWindowContext {
    .inputSystem = inputSystem,
    .windowSystem = windowSystem
  };

  // Set GLFW window context to application
  glfwSetWindowUserPointer(*window, &glfwWindowContext);

  // Run application
  std::expected<void, Error> result;

  result = applicationPtr->onCreate(engineContext);
  if (!result.has_value()) {
    return std::unexpected(error("application on create failed", result.error()));
  }

  while(!glfwWindowShouldClose(*window)) {
    timeSystem.onUpdate();
    
    result = applicationPtr->onUpdate(engineContext);
    if (!result.has_value()) {
      std::cerr << "application on update failed. closing main loop\n\t" << result.error() << '\n';
      break;
    }

    glfwSwapBuffers(*window);
    glfwPollEvents();
  }

  result = applicationPtr->onDestroy(engineContext);
  if (!result.has_value()) {
    return std::unexpected(error("application on destroy failed", result.error()));
  }

  return {};
}

}
