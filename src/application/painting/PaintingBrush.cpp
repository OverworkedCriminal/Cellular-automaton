#include "application/painting/PaintingBrush.hpp"
#include <algorithm>

auto PaintingBrush::create(
  WindowSize simulationSize,
  uint32_t simulationPadding,
  WindowSize framebufferSize,
  uint32_t canvasValueOffset,
  uint32_t canvasValueStride
) -> PaintingBrush {
  return PaintingBrush(
    simulationSize,
    simulationPadding,
    framebufferSize,
    canvasValueOffset,
    canvasValueStride
  );
}

PaintingBrush::PaintingBrush(
  WindowSize simulationSize,
  uint32_t simulationPadding,
  WindowSize framebufferSize,
  uint32_t canvasValueOffset,
  uint32_t canvasValueStride
)
  :m_brushValue(0)
  ,m_brushSize(1)
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

  const int32_t lBoundX = m_simulationPadding;
  const int32_t uBoundX = simulationWidth - m_simulationPadding;

  const int32_t lBoundY = m_simulationPadding;
  const int32_t uBoundY = simulationHeight - m_simulationPadding;

  const int32_t radius = m_brushSize - 1;

  for (int32_t row = -radius; row <= radius; ++row) {
    for (int32_t col = -radius; col <= radius; ++col) {
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

auto PaintingBrush::setSize(uint8_t size) -> void {
  m_brushSize = std::max(size, static_cast<uint8_t>(1));
}

auto PaintingBrush::getSize() const -> uint8_t {
  return m_brushSize;
}

auto PaintingBrush::setFramebufferSize(WindowSize size) -> void {
  m_framebufferSize = size;
}