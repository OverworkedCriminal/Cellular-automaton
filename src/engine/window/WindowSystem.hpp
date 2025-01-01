#ifndef ENGINE_WINDOW_WINDOW_SYSTEM_HPP
#define ENGINE_WINDOW_WINDOW_SYSTEM_HPP

#include "engine/window/IWindowSystem.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <memory>
#include <vector>

namespace engine::window {

class WindowSystem :public IWindowSystem {
public:
  static auto create(GLFWwindow* window) -> WindowSystem;

  WindowSystem(const WindowSystem&) = delete;
  WindowSystem(WindowSystem&&) = default;

  auto operator=(const WindowSystem&) -> WindowSystem& = delete;
  auto operator=(WindowSystem&&) -> WindowSystem& = default;

  auto getFramebufferSize() const -> Size2D<uint32_t> override;

  auto addFrabufferSizeCallback(std::weak_ptr<IFramebufferSizeCallback>) -> void override;

  auto framebufferSizeCallback(GLFWwindow* window, int width, int height) -> void;

private:
  WindowSystem(GLFWwindow* window);

  GLFWwindow* m_window;

  std::vector<std::weak_ptr<IFramebufferSizeCallback>> m_framebufferSizeCallbacks;
};

}

#endif