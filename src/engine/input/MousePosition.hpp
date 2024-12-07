#ifndef ENGINE_INPUT_MOUSE_POSITION_HPP
#define ENGINE_INPUT_MOUSE_POSITION_HPP

#include <cstdint>

namespace engine::input {

struct MousePosition {
  uint32_t x;
  uint32_t y;
};

}

#endif