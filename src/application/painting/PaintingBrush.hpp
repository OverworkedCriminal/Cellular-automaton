#ifndef APPLICATION_PAINTING_PAINTING_BRUSH_HPP
#define APPLICATION_PAINTING_PAINTING_BRUSH_HPP

#include "engine/input/MousePosition.hpp"
#include "engine/window/WindowSize.hpp"
#include <cstdint>
#include <vector>

class PaintingBrush {
public:
  static auto create(
    uint8_t initialValue,
    WindowSize simulationSize,
    uint32_t simulationPadding,
    WindowSize framebufferSize,
    uint32_t canvasValueOffset,
    uint32_t canvasValueStride
  ) -> PaintingBrush;

  PaintingBrush(const PaintingBrush&) = default;
  PaintingBrush(PaintingBrush&&) = default;

  auto operator=(const PaintingBrush&) -> PaintingBrush& = default;
  auto operator=(PaintingBrush&&) -> PaintingBrush& = default;

  auto paint(
    std::vector<uint8_t>& canvas,
    engine::input::MousePosition mousePosition
  ) const -> void;

  auto setValue(uint8_t value) -> void;
  auto setFramebufferSize(WindowSize size) -> void;

private:
  PaintingBrush(
    uint8_t initialValue,
    WindowSize simulationSize,
    uint32_t simulationPadding,
    WindowSize framebufferSize,
    uint32_t canvasValueOffset,
    uint32_t canvasValueStride
  );

  // value that will be used to update canvas
  uint8_t m_brushValue;

  WindowSize m_simulationSize;
  uint32_t m_simulationPadding;

  WindowSize m_framebufferSize;

  uint32_t m_canvasValueOffset;
  uint32_t m_canvasValueStride;
};

#endif