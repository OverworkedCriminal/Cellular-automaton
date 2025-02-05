#ifndef ENGINE_WINDOW_IWINDOW_SYSTEM_HPP
#define ENGINE_WINDOW_IWINDOW_SYSTEM_HPP

#include "engine/utils/dto/Size2D.hpp"
#include "engine/callback/IFramebufferSizeCallback.hpp"
#include <cstdint>
#include <memory>

namespace engine::window {

class IWindowSystem {
public:
  virtual ~IWindowSystem() {}

  virtual auto getFramebufferSize() const -> Size2D<uint32_t> = 0;

  virtual auto addFrabufferSizeCallback(std::weak_ptr<IFramebufferSizeCallback>) -> void = 0;
};

}

#endif