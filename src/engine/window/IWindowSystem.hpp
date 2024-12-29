#ifndef ENGINE_WINDOW_IWINDOW_SYSTEM_HPP
#define ENGINE_WINDOW_IWINDOW_SYSTEM_HPP

#include "engine/window/WindowSize.hpp"
#include "engine/callback/IFramebufferSizeCallback.hpp"
#include <memory>

namespace engine::window {

class IWindowSystem {
public:
  virtual ~IWindowSystem() {}

  virtual auto getFramebufferSize() const -> WindowSize = 0;

  virtual auto addFrabufferSizeCallback(std::weak_ptr<IFramebufferSizeCallback>) -> void = 0;
};

}

#endif