#ifndef APPLICATION_PAINTING_PAINTING_BRUSH_HPP
#define APPLICATION_PAINTING_PAINTING_BRUSH_HPP

#include "engine/EngineContext.hpp"
#include <cstdint>
#include <vector>

class PaintingBrush {
public:
  static auto create(
    uint8_t initialValue,
    uint32_t simulationWidth,
    uint32_t simulationHeight,
    uint32_t simulationPadding,
    uint32_t canvasValueOffset,
    uint32_t canvasValueStride
  ) -> PaintingBrush;

  PaintingBrush(const PaintingBrush&) = default;
  PaintingBrush(PaintingBrush&&) = default;

  auto operator=(const PaintingBrush&) -> PaintingBrush& = default;
  auto operator=(PaintingBrush&&) -> PaintingBrush& = default;

  auto paint(
    const engine::EngineContext& context,
    std::vector<uint8_t>& canvas
  ) const -> void;

  auto setValue(uint8_t value) -> void;

private:
  PaintingBrush(
    uint8_t initialValue,
    uint32_t simulationWidth,
    uint32_t simulationHeight,
    uint32_t simulationPadding,
    uint32_t canvasValueOffset,
    uint32_t canvasValueStride
  );

  // value that will be used to update canvas
  uint8_t m_brushValue;

  uint32_t m_simulationWidth;
  uint32_t m_simulationHeight;
  uint32_t m_simulationPadding;

  uint32_t m_canvasValueOffset;
  uint32_t m_canvasValueStride;
};

#endif