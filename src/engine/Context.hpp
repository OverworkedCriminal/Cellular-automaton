#ifndef APPLICATION_CONTEXT_HPP
#define APPLICATION_CONTEXT_HPP

#include "engine/input/KeyboardKey.hpp"

namespace engine {

struct Context {
  unsigned int framebufferWidth;
  unsigned int framebufferHeight;

  engine::KeyboardKey keyboardLastKeyPressed;

  unsigned int mousePosX;
  unsigned int mousePosY;
  bool mouseLeftPressed;
};

}

#endif