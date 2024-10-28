#ifndef ENGINE_APPLICATION_IAPPLICATION_HPP
#define ENGINE_APPLICATION_IAPPLICATION_HPP

#include <expected>
#include <string>

namespace engine {

class IApplication {
public:
  virtual ~IApplication() {}

  virtual auto onCreate() -> std::expected<void, std::string> { return {}; }
  virtual auto onDestroy() -> std::expected<void, std::string> { return {}; }
  virtual auto onUpdate() -> std::expected<void, std::string> { return {}; }
};

} // namespace engine

#endif