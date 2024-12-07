#ifndef ENGINE_INPUT_CALLBACK_IMOUSE_POSITION_CALLBACK_HPP
#define ENGINE_INPUT_CALLBACK_IMOUSE_POSITION_CALLBACK_HPP

namespace engine::input {

class IMousePositionCallback {
public:
  virtual ~IMousePositionCallback() {}

  virtual auto onPositionEvent(unsigned posX, unsigned posY) -> void;
};

}

#endif