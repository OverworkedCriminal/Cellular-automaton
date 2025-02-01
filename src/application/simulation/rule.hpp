#ifndef APPLICATION_SIMULATION_RULE_HPP
#define APPLICATION_SIMULATION_RULE_HPP

#include "application/simulation/cell.hpp"
#include <array>
#include <cstdint>

namespace rule {

constexpr std::array<cell_t, 7> VERTICAL = {
  0b00000000, // PADDING
  0b01100000, // AIR
  0b01111010, // SAND
  0b01100010, // WATER_L
  0b01100010, // WATER_R
  0b00000000, // SMOKE_L
  0b00000000  // SMOKE_R
};

constexpr std::array<cell_t, 7> DIAGONAL = {
  0b00000000, // PADDING
  0b01100000, // AIR
  0b01100010, // SAND
  0b01100010, // WATER_L
  0b01100010, // WATER_R
  0b00000000, // SMOKE_L
  0b00000000  // SMOKE_R
};

constexpr std::array<cell_t, 7> HORIZONTAL = {
  0b00000000, // PADDING
  0b00000000, // AIR
  0b00000000, // SAND
  0b00010010, // WATER_L
  0b00001010, // WATER_R
  0b01000010, // SMOKE_L
  0b00100010  // SMOKE_R
};

constexpr std::array<int32_t, 7> HORIZONTAL_DIRECTIONS = {
   0, // PADDING
   0, // AIR
   0, // SAND
  -1, // WATER_L
   1, // WATER_R
  -1, // SMOKE_L
   1  // SMOKE_R
};

constexpr std::array<cell_t, 7> HORIZONTAL_OPPOSITE_DIRECTION_CELL = {
  cell::PADDING, // PADDING
  cell::AIR,     // AIR
  cell::SAND,    // SAND
  cell::WATER_R, // WATER_L
  cell::WATER_L, // WATER_R
  cell::SMOKE_R, // SMOKE_L
  cell::SMOKE_L  // SMOKE_R
};

}

#endif