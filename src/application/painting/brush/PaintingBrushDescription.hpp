#ifndef APPLICATION_PAINTING_BRUSH_PAINTING_BRUSH_DESCRIPTION_HPP
#define APPLICATION_PAINTING_BRUSH_PAINTING_BRUSH_DESCRIPTION_HPP

#include "application/simulation/cell.hpp"
#include <cstdint>

struct PaintingBrushDescription {
  uint8_t size;
  cell_t cell;
};

#endif