#include "engine/utils/error.hpp"

namespace engine {

auto errorGL(std::string&& functionName, GLenum error) -> Error {
  return Error::create(std::format("{} failed: 0x{:04x}", functionName, error));
}

auto error(std::string&& message) -> Error {
  return Error::create(std::forward<std::string&&>(message));
}

auto error(std::string&& message, Error& cause) -> Error {
  return Error::create(std::forward<std::string&&>(message), std::move(cause));
}

}