#ifndef ENGINE_CALLBACK_IFRAMEBUFFER_SIZE_CALLBACK_HPP
#define ENGINE_CALLBACK_IFRAMEBUFFER_SIZE_CALLBACK_HPP

#include "engine/window/WindowSize.hpp"

namespace engine {

class IFramebufferSizeCallback {
public:
  virtual ~IFramebufferSizeCallback() {}

  virtual auto onSizeEvent(WindowSize size) -> void = 0;
};

}

#endif