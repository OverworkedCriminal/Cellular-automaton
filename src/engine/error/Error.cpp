#include "engine/error/Error.hpp"

namespace engine {

auto Error::create(std::string&& message) -> Error {
  return Error(std::move(message));
}

auto Error::create(std::string&& message, Error&& cause) -> Error {
  message += "\n\t";
  message += cause.m_message;
  return Error(std::move(message));
}

Error::Error(std::string&& message)
  :m_message(message)
{}

auto Error::message() -> const std::string& {
  return m_message;
}

auto operator<<(std::ostream& out, Error& error) -> std::ostream& {
  out << error.message();
  return out;
}

}