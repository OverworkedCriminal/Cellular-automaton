#ifndef ENGINE_INPUT_CALLBACK_MOUSE_BUTTON_CALLBACK_HPP
#define ENGINE_INPUT_CALLBACK_MOUSE_BUTTON_CALLBACK_HPP

#include "engine/input/binding/MouseButton.hpp"
#include "engine/input/callback/IMouseButtonCallback.hpp"
#include <functional>

namespace engine::input {

class MouseButtonCallback :public IMouseButtonCallback {
public:
  static auto create(
    std::function<void(MouseButton, bool)> callbackFn
  ) -> MouseButtonCallback;

  MouseButtonCallback(const MouseButtonCallback&) = delete;
  MouseButtonCallback(MouseButtonCallback&&) = default;

  auto operator=(const MouseButtonCallback&) -> MouseButtonCallback& = delete;
  auto operator=(MouseButtonCallback&&) -> MouseButtonCallback& = default;

  auto onButtonEvent(MouseButton button, bool pressed) -> void override;

private:
  MouseButtonCallback(std::function<void(MouseButton, bool)> callbackFn);

  std::function<void(MouseButton, bool)> m_callbackFn;
};

}

#endif