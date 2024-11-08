#ifndef ENGINE_CONTEXT_HPP
#define ENGINE_CONTEXT_HPP

namespace engine {

struct Context {
  int mousePosX;
  int mousePosY;
  bool mousePressed;

  bool key1Pressed;
  bool key2Pressed;
  bool key3Pressed;
};

}


#endif