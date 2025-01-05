#include "application/painting/brush/SquarePaintingBrush.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/utils/position_mapping.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <cassert>

using engine::Position2D;

auto SquarePaintingBrush::create() -> SquarePaintingBrush {
  return SquarePaintingBrush();
}

SquarePaintingBrush::SquarePaintingBrush() {}

auto SquarePaintingBrush::paint(
  const PaintingBrushDescription brushDescription,
  const engine::Position2D<uint32_t> position,
  const PaintingCanvasDescription& canvasDescription,
  std::vector<uint8_t>& canvas
) const -> void {
  const auto [size, padding, valueOffset, valueStride] = canvasDescription;
  const auto [posX, posY] = position;

  assert(canvas.size() == (size.width * size.height * valueStride));

  const int32_t lowerBoundX = padding;
  const int32_t upperBoundX = size.width - padding;

  const int32_t lowerBoundY = padding;
  const int32_t upperBoundY = size.height - padding;

  const int32_t radius = brushDescription.size - 1;

  for (int32_t row = -radius; row <= radius; ++row) {
    for (int32_t col = -radius; col <= radius; ++col) {
      const int32_t x = posX + col;
      const int32_t y = posY + row;
      if (x < lowerBoundX || x >= upperBoundX || y < lowerBoundY || y >= upperBoundY) {
        continue;
      }

      const uint32_t idx = mapSimulationPositionToCanvasIndex(
        { 
          .x = static_cast<uint32_t>(x),
          .y = static_cast<uint32_t>(y)
        },
        canvasDescription
      );

      canvas[idx] = brushDescription.cell;
    }
  }
}
