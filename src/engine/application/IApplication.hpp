#ifndef ENGINE_APPLICATION_IAPPLICATION_HPP
#define ENGINE_APPLICATION_IAPPLICATION_HPP

#include "engine/Context.hpp"
#include <expected>
#include <string>

namespace engine {

class IApplication {
public:
  virtual ~IApplication() {}

  virtual auto onCreate(
    const Context& context
  ) -> std::expected<void, std::string> { return {}; }

  virtual auto onDestroy(
    const Context& context
  ) -> std::expected<void, std::string> { return {}; }

  virtual auto onUpdate(
    const Context& context
  ) -> std::expected<void, std::string> { return {}; }
};

} // namespace engine

#endif