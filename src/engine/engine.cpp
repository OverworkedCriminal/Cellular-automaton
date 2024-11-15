#include "engine/engine.hpp"
#include "engine/Config.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/application/KeyboardKey.hpp"
#include "engine/application/MouseButton.hpp"
#include "engine/utils/error.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

namespace engine {

static auto keyboardCallback(
  GLFWwindow* window,
  int key,
  int scancode,
  int action,
  int mods
) -> void {
  auto applicationPtr = reinterpret_cast<IApplication*>(glfwGetWindowUserPointer(window));

  switch (action) {
    case GLFW_RELEASE:
    case GLFW_PRESS: {
      // assertions make sure code does not compile
      // when GLFW_RELEASE / GLFW_PRESS values change
      static_assert(static_cast<bool>(GLFW_RELEASE) == false, "GLFW_RELEASE must be false");
      static_assert(static_cast<bool>(GLFW_PRESS) == true, "GLFW_PRESS must be true");

      applicationPtr->onKeyboardInput(
        static_cast<KeyboardKey>(key),
        static_cast<bool>(action)
      );
    }
  }
}

static auto mousePositionCallback(
  GLFWwindow* window,
  double posX,
  double posY
) -> void {
  auto applicationPtr = reinterpret_cast<IApplication*>(glfwGetWindowUserPointer(window));
  
  applicationPtr->onMouseMoveInput(
    static_cast<unsigned int>(posX), 
    static_cast<unsigned int>(posY)
  );
}

static auto mouseButtonCallback(
  GLFWwindow* window,
  int button,
  int action,
  int mods
) -> void {
  auto applicationPtr = reinterpret_cast<IApplication*>(glfwGetWindowUserPointer(window));

  static_assert(static_cast<bool>(GLFW_RELEASE) == false, "GLFW_RELEASE must be false");
  static_assert(static_cast<bool>(GLFW_PRESS) == true, "GLFW_PRESS must be true");

  applicationPtr->onMouseButtonInput(
    static_cast<MouseButton>(button), 
    static_cast<bool>(action)
  );
}

static auto framebufferSizeCallback(
  GLFWwindow* window,
  int width,
  int height
) -> void {
  auto applicationPtr = reinterpret_cast<IApplication*>(glfwGetWindowUserPointer(window));

  glViewport(0, 0, width, height);
  applicationPtr->onFramebufferSizeChange(width, height);
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
  std::unique_ptr<IApplication> applicationPtr
) -> std::expected<void, Error> {
  std::expected<void, Error> result;
  
  result = applicationPtr->onCreate();
  if (!result.has_value()) {
    return std::unexpected(error("application on create failed", result.error()));
  }

  // Makes sure correct framebuffer size is used
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  framebufferSizeCallback(window, width, height);

  while(!glfwWindowShouldClose(window)) {
    result = applicationPtr->onUpdate();
    if (!result.has_value()) {
      std::cerr << "application on update failed. closing main loop\n\t" << result.error() << '\n';
      break;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  result = applicationPtr->onDestroy();
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

  glfwSetWindowUserPointer(*window, applicationPtr.get());

  auto runApplicationResult = runApplication(*window, std::move(applicationPtr));

  destroyGLFW();

  return runApplicationResult;
}

}
