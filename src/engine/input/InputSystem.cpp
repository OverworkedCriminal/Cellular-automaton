#include "engine/input/InputSystem.hpp"
#include "engine/callback/callback.hpp"
#include "engine/input/MousePosition.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <cassert>
#include <cstdint>

namespace engine::input {

/**
 * @brief 
 * clamp position to framebuffer size and invert y position
 * 
 * @param window 
 * @param posX 
 * @param posY 
 * @return MousePosition 
 */
static auto processMousePosition(GLFWwindow* window, double posX, double posY) -> MousePosition {
  int x = static_cast<int>(posX);
  int y = static_cast<int>(posY);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  x = std::clamp(x, 0, width - 1);
  y = std::clamp(y, 0, height - 1);

  // mouse position is relative to upper left corner
  // with this change it will be relative to lower left corner
  y = height - 1 - y;

  return MousePosition {
    .x = static_cast<uint32_t>(x),
    .y = static_cast<uint32_t>(y)
  };
}

auto InputSystem::create(GLFWwindow *window) -> InputSystem {
  return InputSystem(window);
}

InputSystem::InputSystem(GLFWwindow *window)
  :m_window(window)
{}

auto InputSystem::isKeyboardKeyPressed(KeyboardKey key) const -> bool {
  int state = glfwGetKey(m_window, static_cast<int>(key));
  return state == GLFW_PRESS;
}

auto InputSystem::isMouseButtonPressed(MouseButton button) const -> bool {
  int state = glfwGetMouseButton(m_window, static_cast<int>(button));
  return state == GLFW_PRESS;
}

auto InputSystem::getMousePosition() const -> MousePosition {
  double posX, posY;
  glfwGetCursorPos(m_window, &posX, &posY);

  const MousePosition mousePosition = processMousePosition(m_window, posX, posY);

  return mousePosition;
}

auto InputSystem::addKeyboardKeyCallback(std::weak_ptr<IKeyboardKeyCallback> callback) -> void {
  m_keyboardKeyCallbacks.emplace_back(callback);
}

auto InputSystem::addMousePositionCallback(std::weak_ptr<IMousePositionCallback> callback) -> void {
  m_mousePositionCallbacks.emplace_back(callback);
}

auto InputSystem::addMouseButtonCallback(std::weak_ptr<IMouseButtonCallback> callback) -> void {
  m_mouseButtonCallbacks.emplace_back(callback);
}

auto InputSystem::keyboardCallback(
  GLFWwindow* window,
  int key,
  int scancode,
  int action,
  int mods
) -> void {
  static_assert(GLFW_PRESS == 1);
  static_assert(GLFW_RELEASE == 0);
  assert(window == m_window);

  switch (action) {
    case GLFW_RELEASE:
    case GLFW_PRESS:
      removeDeadCallbacks(m_keyboardKeyCallbacks);
      for (const auto& callbackWeakPtr : m_keyboardKeyCallbacks) {
        const auto ptr = callbackWeakPtr.lock();
        ptr->onKeyEvent(static_cast<KeyboardKey>(key), action);
      }
      break;
  }
}

auto InputSystem::mousePositionCallback(
  GLFWwindow* window,
  double posX,
  double posY
) -> void {
  assert(window == m_window);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  MousePosition position = processMousePosition(window, posX, posY);

  removeDeadCallbacks(m_mousePositionCallbacks);
  for (const auto& callbackWeakPtr : m_mousePositionCallbacks) {
    const auto ptr = callbackWeakPtr.lock();
    ptr->onPositionEvent(position);
  }
}

auto InputSystem::mouseButtonCallback(
  GLFWwindow* window,
  int button,
  int action,
  int mods
) -> void {
  static_assert(GLFW_PRESS == 1);
  static_assert(GLFW_RELEASE == 0);
  assert(window == m_window);

  switch (action) {
    case GLFW_RELEASE:
    case GLFW_PRESS:
      removeDeadCallbacks(m_mouseButtonCallbacks);
      for (const auto& callbackWeakPtr : m_mouseButtonCallbacks) {
        const auto ptr = callbackWeakPtr.lock();
        ptr->onButtonEvent(static_cast<MouseButton>(button), action);
      }
      break;
  }
}

}