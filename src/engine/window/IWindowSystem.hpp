#ifndef ENGINE_WINDOW_IWINDOW_SYSTEM_HPP
#define ENGINE_WINDOW_IWINDOW_SYSTEM_HPP

#include "engine/window/WindowSize.hpp"

namespace engine::window {

class IWindowSystem {
public:
  virtual ~IWindowSystem() {}

  virtual auto getFramebufferSize() const -> WindowSize;
};

}

#endif