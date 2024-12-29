#ifndef ENGINE_CALLBACK_IMOUSE_POSITION_CALLBACK_HPP
#define ENGINE_CALLBACK_IMOUSE_POSITION_CALLBACK_HPP

#include "engine/input/MousePosition.hpp"

namespace engine {

class IMousePositionCallback {
public:
  virtual ~IMousePositionCallback() {}

  virtual auto onPositionEvent(input::MousePosition position) -> void = 0;
};

}

#endif