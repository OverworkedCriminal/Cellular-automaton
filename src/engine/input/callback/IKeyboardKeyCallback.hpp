#ifndef ENGINE_INPUT_CALLBACK_IKEYBOARD_KEY_CALLBACK_HPP
#define ENGINE_INPUT_CALLBACK_IKEYBOARD_KEY_CALLBACK_HPP

#include "engine/input/binding/KeyboardKey.hpp"

namespace engine::input {

class IKeyboardKeyCallback {
public:
  virtual ~IKeyboardKeyCallback() {}

  virtual auto onKeyEvent(KeyboardKey key, bool pressed) -> void = 0;
};

}

#endif