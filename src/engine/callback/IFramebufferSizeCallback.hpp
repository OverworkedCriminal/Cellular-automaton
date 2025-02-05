#ifndef ENGINE_CALLBACK_IFRAMEBUFFER_SIZE_CALLBACK_HPP
#define ENGINE_CALLBACK_IFRAMEBUFFER_SIZE_CALLBACK_HPP

#include "engine/utils/dto/Size2D.hpp"
#include <cstdint>

namespace engine {

class IFramebufferSizeCallback {
public:
  virtual ~IFramebufferSizeCallback() {}

  virtual auto onSizeEvent(Size2D<uint32_t> size) -> void = 0;
};

}

#endif