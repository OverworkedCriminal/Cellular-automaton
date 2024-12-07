#ifndef ENGINE_INPUT_BINDING_MOUSE_BUTTON_HPP
#define ENGINE_INPUT_BINDING_MOUSE_BUTTON_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace engine::input {

enum class MouseButton {
  LEFT = GLFW_MOUSE_BUTTON_LEFT,
  RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
  _
};

}

#endif