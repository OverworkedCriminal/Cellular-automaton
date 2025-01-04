#include "application/painting/PaintingBrush.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/painting.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <algorithm>
#include <cassert>

using engine::Position2D;

auto PaintingBrush::create(uint8_t size, uint8_t value) -> PaintingBrush {
  return PaintingBrush(
    size,
    value
  );
}

PaintingBrush::PaintingBrush(uint8_t size, uint8_t value)
  :m_brushValue(value)
  ,m_brushSize(size)
{}

auto PaintingBrush::paint(
  std::vector<uint8_t>& canvas,
  const PaintingCanvasDescription& canvasDescription,
  Position2D<uint32_t> position
) const -> void {
  const auto [size, padding, valueOffset, valueStride] = canvasDescription;
  const auto [posX, posY] = position;

  assert(canvas.size() == (size.width * size.height * valueStride));

  const int32_t lowerBoundX = padding;
  const int32_t upperBoundX = size.width - padding;

  const int32_t lowerBoundY = padding;
  const int32_t upperBoundY = size.height - padding;

  const int32_t radius = m_brushSize - 1;

  for (int32_t row = -radius; row <= radius; ++row) {
    for (int32_t col = -radius; col <= radius; ++col) {
      const int32_t x = posX + col;
      const int32_t y = posY + row;
      if (x < lowerBoundX || x >= upperBoundX || y < lowerBoundY || y >= upperBoundY) {
        continue;
      }

      const uint32_t idx = mapPositionToCanvasIndex(
        { 
          .x = static_cast<uint32_t>(x),
          .y = static_cast<uint32_t>(y)
        },
        canvasDescription
      );

      canvas[idx] = m_brushValue;
    }
  }
}

auto PaintingBrush::setValue(uint8_t value) -> void {
  m_brushValue = value;
}

auto PaintingBrush::setSize(uint8_t size) -> void {
  m_brushSize = std::max(size, static_cast<uint8_t>(1));
}

auto PaintingBrush::getSize() const -> uint8_t {
  return m_brushSize;
}
