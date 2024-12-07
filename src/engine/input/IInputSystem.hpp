#ifndef ENGINE_INPUT_IINPUT_SYSTEM_HPP
#define ENGINE_INPUT_IINPUT_SYSTEM_HPP

#include "engine/input/MousePosition.hpp"
#include "engine/input/binding/KeyboardKey.hpp"
#include "engine/input/binding/MouseButton.hpp"
#include "engine/input/callback/IKeyboardKeyCallback.hpp"
#include "engine/input/callback/IMouseButtonCallback.hpp"
#include "engine/input/callback/IMousePositionCallback.hpp"
#include <memory>

namespace engine::input {

class IInputSystem {
public:
  virtual ~IInputSystem() {}

  virtual auto isKeyboardKeyPressed(KeyboardKey key) const -> bool;
  virtual auto isMouseButtonPressed(MouseButton button) const -> bool;
  virtual auto getMousePosition() const -> MousePosition;

  virtual auto addKeyboardKeyCallback(std::weak_ptr<IKeyboardKeyCallback>) -> void;
  virtual auto addMousePositionCallback(std::weak_ptr<IMousePositionCallback>) -> void;
  virtual auto addMouseButtonCallback(std::weak_ptr<IMouseButtonCallback>) -> void;
};

}


#endif