#ifndef ENGINE_INPUT_CALLBACK_MOUSE_POSITION_CALLBACK_HPP
#define ENGINE_INPUT_CALLBACK_MOUSE_POSITION_CALLBACK_HPP

#include "engine/input/MousePosition.hpp"
#include "engine/input/callback/IMousePositionCallback.hpp"
#include <functional>

namespace engine::input {

class MousePositionCallback :public IMousePositionCallback {
public:
  static auto create(
    std::function<void(MousePosition)> callbackFn
  ) -> MousePositionCallback;

  MousePositionCallback(const MousePositionCallback &) = delete;
  MousePositionCallback(MousePositionCallback &&) = default;

  MousePositionCallback &operator=(const MousePositionCallback &) = delete;
  MousePositionCallback &operator=(MousePositionCallback &&) = default;

  auto onPositionEvent(MousePosition position) -> void override;

private:
  MousePositionCallback(std::function<void(MousePosition)> callbackFn);

  std::function<void(MousePosition)> m_callbackFn;
};

}

#endif