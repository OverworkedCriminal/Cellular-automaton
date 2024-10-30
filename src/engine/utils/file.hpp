#ifndef ENGINE_UTILS_FILE_HPP
#define ENGINE_UTILS_FILE_HPP

#include <expected>
#include <string>

namespace engine {

/**
 * @brief
 * Read file to the string
 *
 * @param filepath
 * @return std::expected<std::string, std::string>
 */
auto read_file(const std::string& filepath) -> std::expected<std::string, std::string>;

}

#endif