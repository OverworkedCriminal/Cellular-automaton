#ifndef APPLICATION_SIMULATION_CPU_UTILS_HPP
#define APPLICATION_SIMULATION_CPU_UTILS_HPP

#include "application/simulation/cell.hpp"
#include <vector>

auto mapCellsToColors(
  const std::vector<cell_t>& inCells,
  std::vector<float>& outColors
) -> void;

#endif