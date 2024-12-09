#ifndef APPLICATION_SIMULATION_CPU_UTILS_HPP
#define APPLICATION_SIMULATION_CPU_UTILS_HPP

#include "application/simulation/Cell.hpp"
#include <vector>

auto mapCellsToColors(
  const std::vector<Cell>& buffer,
  std::vector<float>& textureBuffer
) -> void;

#endif