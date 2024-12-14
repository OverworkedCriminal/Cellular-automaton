#ifndef APPLICATION_SIMULATION_CELL_HPP
#define APPLICATION_SIMULATION_CELL_HPP

#include <cstdint>

enum class Cell : uint8_t {
  PADDING = 1,
  AIR = 2,
  SAND = 4,
  WATER = 8
};

#endif