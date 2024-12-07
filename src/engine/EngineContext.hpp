#ifndef ENGINE_ENGINE_CONTEXT_HPP
#define ENGINE_ENGINE_CONTEXT_HPP

#include "engine/input/IInputSystem.hpp"
#include "engine/window/IWindowSystem.hpp"

namespace engine {

struct EngineContext {
  input::IInputSystem& inputSystem;
  window::IWindowSystem& windowSystem;
};

}

#endif