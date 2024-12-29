#include "application/painting/PaintingBrush.hpp"

auto PaintingBrush::create(
  uint8_t initialValue,
  WindowSize simulationSize,
  uint32_t simulationPadding,
  WindowSize framebufferSize,
  uint32_t canvasValueOffset,
  uint32_t canvasValueStride
) -> PaintingBrush {
  return PaintingBrush(
    initialValue,
    simulationSize,
    simulationPadding,
    framebufferSize,
    canvasValueOffset,
    canvasValueStride
  );
}

PaintingBrush::PaintingBrush(
  uint8_t initialValue,
  WindowSize simulationSize,
  uint32_t simulationPadding,
  WindowSize framebufferSize,
  uint32_t canvasValueOffset,
  uint32_t canvasValueStride
)
  :m_brushValue(initialValue)
  ,m_simulationSize(simulationSize)
  ,m_simulationPadding(simulationPadding)
  ,m_framebufferSize(framebufferSize)
  ,m_canvasValueOffset(canvasValueOffset)
  ,m_canvasValueStride(canvasValueStride)
{}

auto PaintingBrush::paint(
  std::vector<uint8_t>& canvas,
  engine::input::MousePosition mousePosition
) const -> void {
  const auto [simulationWidth, simulationHeight] = m_simulationSize;
  const auto [framebufferWidth, framebufferHeight] = m_framebufferSize;
  const auto [mousePosX, mousePosY] = mousePosition;

  const uint32_t mouseScaledPosX = (static_cast<float>(simulationWidth) / framebufferWidth) * mousePosX;
  const uint32_t mouseScaledPosY = (static_cast<float>(simulationHeight) / framebufferHeight) * mousePosY;

  constexpr int BRUSH_SIZE = 2;

  const int32_t lBoundX = m_simulationPadding;
  const int32_t uBoundX = simulationWidth - m_simulationPadding;

  const int32_t lBoundY = m_simulationPadding;
  const int32_t uBoundY = simulationHeight - m_simulationPadding;

  for (int32_t row = -BRUSH_SIZE; row <= BRUSH_SIZE; ++row) {
    for (int32_t col = -BRUSH_SIZE; col <= BRUSH_SIZE; ++col) {
      int32_t x = mouseScaledPosX + col;
      int32_t y = mouseScaledPosY + row;
      if (x < lBoundX || x >= uBoundX || y < lBoundY || y >= uBoundY) {
        continue;
      }

      int32_t idx = (y * simulationWidth + x) * m_canvasValueStride + m_canvasValueOffset;
      canvas[idx] = m_brushValue;
    }
  }
}

auto PaintingBrush::setValue(uint8_t value) -> void {
  m_brushValue = value;
}

auto PaintingBrush::setFramebufferSize(WindowSize size) -> void {
  m_framebufferSize = size;
}