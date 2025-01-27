#ifndef APPLICATION_SIMULATION_RULE_HPP
#define APPLICATION_SIMULATION_RULE_HPP

#include "application/simulation/cell.hpp"
#include <array>
#include <cstdint>

namespace rule {

constexpr std::array<cell_t, 5> VERTICAL = {
  0b00000000, // PADDING
  0b00000000, // AIR
  0b00011010, // SAND
  0b00000010, // WATER_L
  0b00000010  // WATER_R
};

constexpr std::array<cell_t, 5> DIAGONAL = {
  0b00000000, // PADDING
  0b00000000, // AIR
  0b00000010, // SAND
  0b00000010, // WATER_L
  0b00000010  // WATER_R
};

constexpr std::array<cell_t, 5> HORIZONTAL = {
  0b00000000, // PADDING
  0b00000000, // AIR
  0b00000000, // SAND
  0b00010010, // WATER_L
  0b00001010  // WATER_R
};

constexpr std::array<int32_t, 5> HORIZONTAL_DIRECTIONS = {
   0, // PADDING
   0, // AIR
   0, // SAND
  -1, // WATER_L
   1  // WATER_R
};

constexpr std::array<cell_t, 5> HORIZONTAL_OPPOSITE_DIRECTION_CELL = {
  cell::PADDING, // PADDING
  cell::AIR,     // AIR
  cell::SAND,    // SAND
  cell::WATER_R, // WATER_L
  cell::WATER_L  // WATER_R
};

}

#endif