#ifndef ENGINE_CALLBACK_IMOUSE_BUTTON_CALLBACK_HPP
#define ENGINE_CALLBACK_IMOUSE_BUTTON_CALLBACK_HPP

#include "engine/input/binding/MouseButton.hpp"

namespace engine {

class IMouseButtonCallback {
public:
  virtual ~IMouseButtonCallback() {}

  virtual auto onButtonEvent(input::MouseButton button, bool pressed) -> void = 0;
};

}

#endif