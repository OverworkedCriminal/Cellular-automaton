#include "engine/input/callback/MousePositionCallback.hpp"
#include "engine/input/MousePosition.hpp"

namespace engine::input {

auto MousePositionCallback::create(
  std::function<void (MousePosition)> callbackFn
) -> MousePositionCallback {
  return MousePositionCallback(callbackFn);
}

MousePositionCallback::MousePositionCallback(
  std::function<void (MousePosition)> callbackFn
) 
  :m_callbackFn(callbackFn)
{}

auto MousePositionCallback::onPositionEvent(MousePosition position) -> void {
  m_callbackFn(position);
}

}