#include "engine/input/callback/MouseButtonCallback.hpp"
#include "engine/input/binding/MouseButton.hpp"

namespace engine::input {

auto MouseButtonCallback::create(
  std::function<void (MouseButton, bool)> callbackFn
) -> MouseButtonCallback {
  return MouseButtonCallback(callbackFn);
}

MouseButtonCallback::MouseButtonCallback(
  std::function<void (MouseButton, bool)> callbackFn
)
  :m_callbackFn(callbackFn)
{}

auto MouseButtonCallback::onButtonEvent(MouseButton button, bool pressed) -> void {
  m_callbackFn(button, pressed);
}

}