#ifndef ENGINE_INPUT_BINDING_KEYBOARD_KEY_HPP
#define ENGINE_INPUT_BINDING_KEYBOARD_KEY_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace engine::input {

enum class KeyboardKey : int {
  _1 = GLFW_KEY_1,
  _2 = GLFW_KEY_2,
  _3 = GLFW_KEY_3,
  PLUS = GLFW_KEY_KP_ADD,
  MINUS = GLFW_KEY_KP_SUBTRACT,
  _
};

}

#endif