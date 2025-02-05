#ifndef APPLICATION_SIMULATION_CPU_COLORS_HPP
#define APPLICATION_SIMULATION_CPU_COLORS_HPP

#include "application/simulation/cpu/Color.hpp"
#include <array>

constexpr std::array<Color, 7> COLORS = {
  Color { 0.0f, 0.0f, 0.0f, 1.0f }, // PADDING
  Color { 0.0f, 0.0f, 0.0f, 1.0f }, // AIR
  Color { 1.0f, 1.0f, 0.0f, 1.0f }, // SAND
  Color { 0.0f, 0.0f, 1.0f, 1.0f }, // WATER_L
  Color { 0.0f, 0.0f, 1.0f, 1.0f }, // WATER_R
  Color { 0.2f, 0.2f, 0.2f, 1.0f }, // SMOKE_L
  Color { 0.2f, 0.2f, 0.2f, 1.0f }  // SMOKE_R
};

#endif