#ifndef ENGINE_CALLBACK_IMOUSE_POSITION_CALLBACK_HPP
#define ENGINE_CALLBACK_IMOUSE_POSITION_CALLBACK_HPP

#include "engine/utils/dto/Position2D.hpp"
#include <cstdint>

namespace engine {

class IMousePositionCallback {
public:
  virtual ~IMousePositionCallback() {}

  virtual auto onPositionEvent(Position2D<uint32_t> position) -> void = 0;
};

}

#endif