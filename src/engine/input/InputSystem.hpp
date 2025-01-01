#ifndef ENGINE_INPUT_INPUT_SYSTEM_HPP
#define ENGINE_INPUT_INPUT_SYSTEM_HPP

#include "engine/input/IInputSystem.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <vector>

namespace engine::input {

class InputSystem :public IInputSystem {
public:
  static auto create(GLFWwindow* window) -> InputSystem;

  InputSystem(const InputSystem&) = delete;
  InputSystem(InputSystem&&) = default;

  auto operator=(const InputSystem&) -> InputSystem& = delete;
  auto operator=(InputSystem&&) -> InputSystem& = default;

  auto isKeyboardKeyPressed(KeyboardKey key) const -> bool override;
  auto isMouseButtonPressed(MouseButton button) const -> bool override;
  auto getMousePosition() const -> Position2D<uint32_t> override;

  auto addKeyboardKeyCallback(std::weak_ptr<IKeyboardKeyCallback>) -> void override;
  auto addMousePositionCallback(std::weak_ptr<IMousePositionCallback>) -> void override;
  auto addMouseButtonCallback(std::weak_ptr<IMouseButtonCallback>) -> void override;

  auto keyboardCallback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
  ) -> void;

  auto mousePositionCallback(GLFWwindow* window, double posX, double posY) -> void;
  auto mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) -> void;

private:
  InputSystem(GLFWwindow* window);

  GLFWwindow* m_window;

  std::vector<std::weak_ptr<IKeyboardKeyCallback>> m_keyboardKeyCallbacks;
  std::vector<std::weak_ptr<IMousePositionCallback>> m_mousePositionCallbacks;
  std::vector<std::weak_ptr<IMouseButtonCallback>> m_mouseButtonCallbacks;
};

}

#endif