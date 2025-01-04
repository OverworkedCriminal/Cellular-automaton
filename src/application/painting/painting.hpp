#ifndef APPLICATION_PAINTING_PAINTING_HPP
#define APPLICATION_PAINTING_PAINTING_HPP

#include "application/painting/PaintingCanvasDescription.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include <cstdint>

auto mapPositionToCanvasIndex(
  engine::Position2D<uint32_t> position,
  const PaintingCanvasDescription& canvasDescription
) -> uint32_t;

#endif