#include "application/painting/painting.hpp"

auto mapWindowPositionToSimulationPosition(
  engine::Position2D<uint32_t> windowPosition,
  engine::Size2D<uint32_t> framebufferSize,
  engine::Size2D<uint32_t> simulationSize
) -> engine::Position2D<uint32_t> {
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