#include "engine/window/WindowSystem.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace engine::window {

auto WindowSystem::create(GLFWwindow* window) -> WindowSystem {
  return WindowSystem(window);
}

WindowSystem::WindowSystem(GLFWwindow* window)
  :m_window(window)
{}

auto WindowSystem::getFramebufferSize() const -> WindowSize {
  int width, height;
  glfwGetFramebufferSize(m_window, &width, &height);

  return WindowSize {
    .width = static_cast<uint32_t>(width),
    .height = static_cast<uint32_t>(height)
  };
}

}