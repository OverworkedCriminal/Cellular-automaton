#ifndef APPLICATION_SIMULATION_CELL_HPP
#define APPLICATION_SIMULATION_CELL_HPP

#include <cstdint>

typedef uint32_t cell_t;

namespace cell {
  constexpr cell_t PADDING = 1;
  constexpr cell_t AIR = 2;
  constexpr cell_t SAND = 4;
  constexpr cell_t WATER_L = 8;
  constexpr cell_t WATER_R = 16;
}

#endif