#ifndef ENGINE_INPUT_CALLBACK_IMOUSE_BUTTON_CALLBACK_HPP
#define ENGINE_INPUT_CALLBACK_IMOUSE_BUTTON_CALLBACK_HPP

#include "engine/input/binding/MouseButton.hpp"

namespace engine::input {

class IMouseButtonCallback {
public:
  virtual ~IMouseButtonCallback() {}

  virtual auto onButtonEvent(MouseButton button, bool pressed) -> void = 0;
};

}

#endif