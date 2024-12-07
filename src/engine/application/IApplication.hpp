#ifndef ENGINE_APPLICATION_IAPPLICATION_HPP
#define ENGINE_APPLICATION_IAPPLICATION_HPP

#include "engine/EngineContext.hpp"
#include "engine/error/Error.hpp"
#include <expected>

namespace engine {

class IApplication {
public:
  virtual ~IApplication() {}

  virtual auto onCreate(EngineContext& context) -> std::expected<void, Error> { return {}; }
  virtual auto onDestroy(EngineContext& context) -> std::expected<void, Error> { return {}; }
  virtual auto onUpdate(EngineContext& context) -> std::expected<void, Error> { return {}; }
};

} // namespace engine

#endif