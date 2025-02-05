#ifndef ENGINE_UTILS_ERROR_HPP
#define ENGINE_UTILS_ERROR_HPP

#include "engine/error/Error.hpp"
#include "glad/glad.h"

namespace engine {

/**
 * @brief
 * Utility function that simplifies error creation.
 * Function is meant to wrap openGL error codes.
 * 
 * @param functionName 
 * @param error 
 * @return Error 
 */
auto errorGL(std::string&& functionName, GLenum error) -> Error;

/**
 * @brief
 * Utility function that simplifies error creation.
 * 
 * @param message 
 * @return Error 
 */
auto error(std::string&& message) -> Error;

/**
 * @brief
 * Utility function that simplifies error creation.
 * Despite declaration this function moves cause to created error.
 * 
 * @param message 
 * @return Error 
 */
auto error(std::string&& message, Error& cause) -> Error;

}

#endif