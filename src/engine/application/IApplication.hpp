#ifndef ENGINE_APPLICATION_IAPPLICATION_HPP
#define ENGINE_APPLICATION_IAPPLICATION_HPP

#include "engine/Context.hpp"
#include "engine/error/Error.hpp"
#include <expected>

namespace engine {

class IApplication {
public:
  virtual ~IApplication() {}

  virtual auto onCreate(const Context& context) -> std::expected<void, Error> { return {}; }
  virtual auto onDestroy(const Context& context) -> std::expected<void, Error> { return {}; }
  virtual auto onUpdate(const Context& context) -> std::expected<void, Error> { return {}; }
};

} // namespace engine

#endif