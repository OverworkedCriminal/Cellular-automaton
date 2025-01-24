#include "application/painting/utils/position_mapping.hpp"

using engine::Position2D;
using engine::Size2D;

auto mapWindowPositionToSimulationPosition(
  Position2D<uint32_t> windowPosition,
  Size2D<uint32_t> framebufferSize,
  Size2D<uint32_t> simulationSize
) -> Position2D<uint32_t> {
  const auto [posX, posY] = windowPosition;
  const auto [framebufferWidth, framebufferHeight] = framebufferSize;
  const auto [simulationWidth, simulationHeight] = simulationSize;

  const float scaleWidth = static_cast<float>(simulationWidth) / framebufferWidth;
  const float scaleHeight = static_cast<float>(simulationHeight) / framebufferHeight;

  const uint32_t mappedPosX = scaleWidth * posX;
  const uint32_t mappedPosY = scaleHeight * posY;

  return {
    .x = mappedPosX,
    .y = mappedPosY
  };
}

auto mapSimulationPositionToCanvasIndex(
  Position2D<uint32_t> position,
  const PaintingCanvasDescription& canvasDescription
) -> uint32_t {
  const auto [size, paddingSize, valueOffset, valueStride] = canvasDescription;
  const auto [x, y] = position;

  const uint32_t idx = (y * size.width + x) * valueStride + valueOffset;

  return idx;
}