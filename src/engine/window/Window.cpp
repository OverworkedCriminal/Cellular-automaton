#include "engine/window/Window.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace engine {

auto Window::open(
  const std::string& title,
  int width,
  int height
) -> std::expected<Window, std::string> {
  if (width <= 0 || height <= 0) {
    return std::unexpected("invalid window dimensions");
  }
  if (!glfwInit()) {
    return std::unexpected("failed to init GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* glfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!glfwWindow) {
    glfwTerminate();
    return std::unexpected("failed to create window");
  }

  glfwMakeContextCurrent(glfwWindow);
  glfwSetInputMode(glfwWindow, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    glfwTerminate();
    return std::unexpected("failed to init GLAD");
  }

  glViewport(0, 0, width, height);

  Window window(glfwWindow);

  return window;
}

Window::Window(GLFWwindow* window)
  :m_window(window)
{}

Window::Window(Window&& other) {
  m_window = other.m_window;
  other.m_window = nullptr;
}

Window::~Window() {
  if (m_window != nullptr) {
    glfwTerminate();
  }
}

auto Window::operator=(Window&& other) -> Window& {
  m_window = other.m_window;
  other.m_window = nullptr;
  return *this;
}

auto Window::operator*() -> GLFWwindow* {
  return m_window;
}

}