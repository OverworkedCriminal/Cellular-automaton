#include "application/painting/PaintingBrush.hpp"

auto PaintingBrush::create(
  uint8_t initialValue,
  uint32_t simulationWidth,
  uint32_t simulationHeight,
  uint32_t simulationPadding,
  uint32_t canvasValueOffset,
  uint32_t canvasValueStride
) -> PaintingBrush {
  return PaintingBrush(
    initialValue,
    simulationWidth,
    simulationHeight,
    simulationPadding,
    canvasValueOffset,
    canvasValueStride
  );
}

PaintingBrush::PaintingBrush(
  uint8_t initialValue,
  uint32_t simulationWidth,
  uint32_t simulationHeight,
  uint32_t simulationPadding,
  uint32_t canvasValueOffset,
  uint32_t canvasValueStride
)
  :m_brushValue(initialValue)
  ,m_simulationWidth(simulationWidth)
  ,m_simulationHeight(simulationHeight)
  ,m_simulationPadding(simulationPadding)
  ,m_canvasValueOffset(canvasValueOffset)
  ,m_canvasValueStride(canvasValueStride)
{}

auto PaintingBrush::paint(
  const engine::EngineContext& context,
  std::vector<uint8_t>& canvas
) const -> void {
  const auto& input = context.inputSystem;
  const auto& window = context.windowSystem;

  const auto [framebufferWidth, framebufferHeight] = window.getFramebufferSize();
  const auto [mousePosX, mousePosY] = input.getMousePosition();

  const uint32_t mouseScaledPosX = (static_cast<float>(m_simulationWidth) / framebufferWidth) * mousePosX;
  const uint32_t mouseScaledPosY = (static_cast<float>(m_simulationHeight) / framebufferHeight) * mousePosY;

  constexpr int BRUSH_SIZE = 2;

  const int32_t lBoundX = m_simulationPadding;
  const int32_t uBoundX = m_simulationWidth - m_simulationPadding;

  const int32_t lBoundY = m_simulationPadding;
  const int32_t uBoundY = m_simulationHeight - m_simulationPadding;

  for (int32_t row = -BRUSH_SIZE; row <= BRUSH_SIZE; ++row) {
    for (int32_t col = -BRUSH_SIZE; col <= BRUSH_SIZE; ++col) {
      int32_t x = mouseScaledPosX + col;
      int32_t y = mouseScaledPosY + row;
      if (x < lBoundX || x >= uBoundX || y < lBoundY || y >= uBoundY) {
        continue;
      }

      int32_t idx = (y * m_simulationWidth + x) * m_canvasValueStride + m_canvasValueOffset;
      canvas[idx] = m_brushValue;
    }
  }
}

auto PaintingBrush::setValue(uint8_t value) -> void {
  m_brushValue = value;
}