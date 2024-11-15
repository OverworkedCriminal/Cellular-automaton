#ifndef ENGINE_APPLICATION_IAPPLICATION_HPP
#define ENGINE_APPLICATION_IAPPLICATION_HPP

#include "engine/application/KeyboardKey.hpp"
#include "engine/application/MouseButton.hpp"
#include "engine/error/Error.hpp"
#include <expected>

namespace engine {

class IApplication {
public:
  virtual ~IApplication() {}

  virtual auto onCreate() -> std::expected<void, Error> { return {}; }
  virtual auto onDestroy() -> std::expected<void, Error> { return {}; }
  virtual auto onUpdate() -> std::expected<void, Error> { return {}; }

  virtual auto onKeyboardInput(KeyboardKey key, bool pressed) -> void {}
  virtual auto onMouseMoveInput(unsigned int posX, unsigned int posY) -> void {}
  virtual auto onMouseButtonInput(MouseButton button, bool pressed) -> void {}

  virtual auto onFramebufferSizeChange(unsigned int width, unsigned int height) -> void {};
};

} // namespace engine

#endif