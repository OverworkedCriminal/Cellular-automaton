#ifndef ENGINE_ENGINE_HPP
#define ENGINE_ENGINE_HPP

#include "engine/Config.hpp"
#include "engine/application/IApplication.hpp"
#include <expected>
#include <string>

namespace engine {

auto run(
  const Config& config,
  IApplication& application
) -> std::expected<void, std::string>;

} // namespace engine

#endif