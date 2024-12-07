#ifndef ENGINE_INPUT_CALLBACK_KEYBOARD_KEY_CALLBACK_HPP
#define ENGINE_INPUT_CALLBACK_KEYBOARD_KEY_CALLBACK_HPP

#include "engine/input/callback/IKeyboardKeyCallback.hpp"
#include <functional>

namespace engine::input {

class KeyboardKeyCallback :public IKeyboardKeyCallback {
public:
  static auto create(
    std::function<void(KeyboardKey, bool)> callbackFn
  ) -> KeyboardKeyCallback;

  KeyboardKeyCallback(const KeyboardKeyCallback&) = delete;
  KeyboardKeyCallback(KeyboardKeyCallback&&) = default;

  auto operator=(const KeyboardKeyCallback&) -> KeyboardKeyCallback& = delete;
  auto operator=(KeyboardKeyCallback&&) -> KeyboardKeyCallback& = default;

  auto onKeyEvent(KeyboardKey key, bool pressed) -> void override;

private:
  KeyboardKeyCallback(std::function<void(KeyboardKey, bool)> callbackFn);

  std::function<void(KeyboardKey, bool)> m_callbackFn;
};

}

#endif