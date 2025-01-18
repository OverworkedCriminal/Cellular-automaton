#ifndef APPLICATION_SIMULATION_CELL_HPP
#define APPLICATION_SIMULATION_CELL_HPP

#include <cstdint>

namespace cell {
  constexpr uint8_t PADDING = 1;
  constexpr uint8_t AIR = 2;
  constexpr uint8_t SAND = 4;
  constexpr uint8_t WATER_L = 8;
  constexpr uint8_t WATER_R = 16;
}

#endif