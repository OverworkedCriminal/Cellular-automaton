#ifndef ENGINE_INPUT_IINPUT_SYSTEM_HPP
#define ENGINE_INPUT_IINPUT_SYSTEM_HPP

#include "engine/input/MousePosition.hpp"
#include "engine/input/binding/KeyboardKey.hpp"
#include "engine/input/binding/MouseButton.hpp"
#include "engine/callback/IKeyboardKeyCallback.hpp"
#include "engine/callback/IMouseButtonCallback.hpp"
#include "engine/callback/IMousePositionCallback.hpp"
#include <memory>

namespace engine::input {

class IInputSystem {
public:
  virtual ~IInputSystem() {}

  virtual auto isKeyboardKeyPressed(KeyboardKey key) const -> bool = 0;
  virtual auto isMouseButtonPressed(MouseButton button) const -> bool = 0;
  virtual auto getMousePosition() const -> MousePosition = 0;

  virtual auto addKeyboardKeyCallback(std::weak_ptr<IKeyboardKeyCallback>) -> void = 0;
  virtual auto addMousePositionCallback(std::weak_ptr<IMousePositionCallback>) -> void = 0;
  virtual auto addMouseButtonCallback(std::weak_ptr<IMouseButtonCallback>) -> void = 0;
};

}


#endif