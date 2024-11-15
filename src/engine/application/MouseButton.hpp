#ifndef ENGINE_APPLICATION_MOUSE_BUTTON_HPP
#define ENGINE_APPLICATION_MOUSE_BUTTON_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace engine {

enum class MouseButton : int {
  LEFT = GLFW_MOUSE_BUTTON_LEFT,
  _
};

}

#endif