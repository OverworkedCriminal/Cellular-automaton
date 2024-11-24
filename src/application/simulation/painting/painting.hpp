#ifndef APPLICATION_SIMULATION_PAINTING_HPP
#define APPLICATION_SIMULATION_PAINTING_HPP

#include "application/simulation/Cell.hpp"
#include <vector>

auto paint(
  unsigned int posX,
  unsigned int posY,
  Cell cell,
  std::vector<Cell>& grid,
  unsigned int gridWidth,
  unsigned int gridHeight,
  unsigned int gridPadding,
  unsigned int gridValueOffset,
  unsigned int gridValueStride
) -> void;

#endif