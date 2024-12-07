#include "engine/input/callback/KeyboardKeyCallback.hpp"

namespace engine::input {

auto KeyboardKeyCallback::create(
  std::function<void(KeyboardKey, bool)> callbackFn
) -> KeyboardKeyCallback {
  return KeyboardKeyCallback(callbackFn);
}

KeyboardKeyCallback::KeyboardKeyCallback(
  std::function<void(KeyboardKey, bool)> callbackFn
)
  :m_callbackFn(callbackFn)
{}

auto KeyboardKeyCallback::onKeyEvent(KeyboardKey key, bool pressed) -> void {
  m_callbackFn(key, pressed);
}

}