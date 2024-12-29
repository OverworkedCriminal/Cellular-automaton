#ifndef ENGINE_CALLBACK_IKEYBOARD_KEY_CALLBACK_HPP
#define ENGINE_CALLBACK_IKEYBOARD_KEY_CALLBACK_HPP

#include "engine/input/binding/KeyboardKey.hpp"

namespace engine {

class IKeyboardKeyCallback {
public:
  virtual ~IKeyboardKeyCallback() {}

  virtual auto onKeyEvent(input::KeyboardKey key, bool pressed) -> void = 0;
};

}

#endif