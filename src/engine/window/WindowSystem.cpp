#include "engine/window/WindowSystem.hpp"
#include "engine/callback/callback.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <cassert>

namespace engine::window {

auto WindowSystem::create(GLFWwindow* window) -> WindowSystem {
  return WindowSystem(window);
}

WindowSystem::WindowSystem(GLFWwindow* window)
  :m_window(window)
{}

auto WindowSystem::getFramebufferSize() const -> Size2D<uint32_t> {
  int width, height;
  glfwGetFramebufferSize(m_window, &width, &height);

  return Size2D<uint32_t> {
    .width = static_cast<uint32_t>(width),
    .height = static_cast<uint32_t>(height)
  };
}

auto WindowSystem::addFrabufferSizeCallback(std::weak_ptr<IFramebufferSizeCallback> callback) -> void {
  m_framebufferSizeCallbacks.push_back(std::move(callback));
}

auto WindowSystem::framebufferSizeCallback(GLFWwindow* window, int width, int height) -> void {
  assert(window == m_window);

  glViewport(0, 0, width, height);

  const auto size = Size2D<uint32_t> {
    .width = static_cast<uint32_t>(width),
    .height = static_cast<uint32_t>(height)
  };

  removeDeadCallbacks(m_framebufferSizeCallbacks);
  for (const auto& callbackWeakPtr : m_framebufferSizeCallbacks) {
    const auto ptr = callbackWeakPtr.lock();
    ptr->onSizeEvent(size);
  }
}

}