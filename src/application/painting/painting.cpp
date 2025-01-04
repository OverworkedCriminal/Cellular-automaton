#include "application/painting/painting.hpp"

auto mapPositionToCanvasIndex(
  engine::Position2D<uint32_t> position,
  const PaintingCanvasDescription& canvasDescription
) -> uint32_t {
  const auto [size, paddingSize, valueOffset, valueStride] = canvasDescription;
  const auto [x, y] = position;

  const uint32_t idx = (y * size.width + x) * valueStride + valueOffset;

  return idx;
}