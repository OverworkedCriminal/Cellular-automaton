#include "engine/engine.hpp"
#include "engine/Config.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/application/KeyboardKey.hpp"
#include "engine/utils/error.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <algorithm>
#include <iostream>

namespace engine {

static auto keyboardCallback(
  GLFWwindow* window,
  int key,
  int scancode,
  int action,
  int mods
) -> void {
  auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));

  switch (action) {
    case GLFW_RELEASE:
      break;
    case GLFW_PRESS:
      context->keyboardLastKeyPressed = static_cast<KeyboardKey>(key);
      break;
  }
}

static auto mousePositionCallback(
  GLFWwindow* window,
  double posX,
  double posY
) -> void {
  auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));

  unsigned mousePosX = std::clamp(static_cast<int>(posX), 0, static_cast<int>(context->framebufferWidth));
  unsigned mousePosY = std::clamp(static_cast<int>(posY), 0, static_cast<int>(context->framebufferHeight));

  context->mousePosX = mousePosX;
  context->mousePosY = context->framebufferHeight - 1 - mousePosY;
}

static auto mouseButtonCallback(
  GLFWwindow* window,
  int button,
  int action,
  int mods
) -> void {
  static_assert(static_cast<bool>(GLFW_RELEASE) == false, "GLFW_RELEASE must be false");
  static_assert(static_cast<bool>(GLFW_PRESS) == true, "GLFW_PRESS must be true");

  auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));

  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      context->mouseLeftPressed = action;
      break;
  }
}

static auto framebufferSizeCallback(
  GLFWwindow* window,
  int width,
  int height
) -> void {
  auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));

  glViewport(0, 0, width, height);
  context->framebufferWidth = width;
  context->framebufferHeight = height;
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

auto runApplication(
  GLFWwindow* window,
  Context& engineContext,
  std::unique_ptr<IApplication> applicationPtr
) -> std::expected<void, Error> {
  std::expected<void, Error> result;

  result = applicationPtr->onCreate(engineContext);
  if (!result.has_value()) {
    return std::unexpected(error("application on create failed", result.error()));
  }

  while(!glfwWindowShouldClose(window)) {
    result = applicationPtr->onUpdate(engineContext);
    if (!result.has_value()) {
      std::cerr << "application on update failed. closing main loop\n\t" << result.error() << '\n';
      break;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  result = applicationPtr->onDestroy(engineContext);
  if (!result.has_value()) {
    return std::unexpected(error("application on destroy failed", result.error()));
  }

  return {};
}

auto run(
  const Config& config,
  std::unique_ptr<IApplication> applicationPtr
) -> std::expected<void, Error> {
  auto window = initGLFW(config.windowTitle, config.windowWidth, config.windowHeight);
  if (!window.has_value()) {
    return std::unexpected(error("failed to init window", window.error()));
  }

  Context engineContext;

  // Set GLFW window context to application
  glfwSetWindowUserPointer(*window, &engineContext);

  // Init application

  int width, height;
  glfwGetFramebufferSize(*window, &width, &height);
  framebufferSizeCallback(*window, width, height);

  double posX, posY;
  glfwGetCursorPos(*window, &posX, &posY);
  mousePositionCallback(*window, posX, posY);

  keyboardCallback(*window, GLFW_KEY_1, 0, GLFW_PRESS, 0);

  // Run application
  auto runApplicationResult = runApplication(
    *window,
    engineContext,
    std::move(applicationPtr)
  );

  destroyGLFW();

  return runApplicationResult;
}

}
