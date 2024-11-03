#ifndef ENGINE_ENGINE_HPP
#define ENGINE_ENGINE_HPP

#include "engine/Config.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/error/Error.hpp"
#include <expected>
#include <memory>

namespace engine {

auto run(
  const Config& config,
  std::unique_ptr<IApplication> application
) -> std::expected<void, Error>;

} // namespace engine

#endif