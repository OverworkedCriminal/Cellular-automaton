#include "engine/utils/file.hpp"

#include <fstream>
#include <ios>

namespace engine {

auto read_file(const std::string& path) -> std::expected<std::string, std::string> {
  // Open file at the end to know its size
  std::ifstream file(path, std::ios_base::in | std::ios_base::ate);
  if (!file.is_open()) {
    return std::unexpected("failed to read file: " + path);
  }

  std::string content;
  content.resize(file.tellg());

  file.seekg(std::ios::beg);
  file.read(content.data(), content.size());

  return content;
}

}