#ifndef ENGINE_GLFW_WINDOW_CONTEXT_HPP
#define ENGINE_GLFW_WINDOW_CONTEXT_HPP

#include "engine/input/InputSystem.hpp"
#include "engine/window/WindowSystem.hpp"

namespace engine {

struct GlfwWindowContext {
  input::InputSystem& inputSystem;
  window::WindowSystem& windowSystem;
};

}

#endif