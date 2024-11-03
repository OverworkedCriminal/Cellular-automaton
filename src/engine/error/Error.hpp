#ifndef ENGINE_ERROR_ERROR_HPP
#define ENGINE_ERROR_ERROR_HPP

#include <ostream>
#include <string>

namespace engine {

class Error {
public:
  static auto create(std::string&& message) -> Error;
  static auto create(std::string&& message, Error&& cause) -> Error;

  Error(const Error&) = delete;
  Error(Error&&) = default;

  auto operator=(const Error&) -> Error& = delete;
  auto operator=(Error&&) -> Error& = default;

  auto message() -> const std::string&;

private:
  Error(std::string&& message);

  std::string m_message;
};

auto operator<<(std::ostream& out, Error& error) -> std::ostream&;

}

#endif