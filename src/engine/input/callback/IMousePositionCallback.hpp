#ifndef ENGINE_INPUT_CALLBACK_IMOUSE_POSITION_CALLBACK_HPP
#define ENGINE_INPUT_CALLBACK_IMOUSE_POSITION_CALLBACK_HPP

#include "engine/input/MousePosition.hpp"

namespace engine::input {

class IMousePositionCallback {
public:
  virtual ~IMousePositionCallback() {}

  virtual auto onPositionEvent(MousePosition position) -> void;
};

}

#endif