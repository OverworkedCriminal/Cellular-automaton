#ifndef APPLICATION_PAINTING_PAINTING_BRUSH_HPP
#define APPLICATION_PAINTING_PAINTING_BRUSH_HPP

#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <cstdint>
#include <vector>

/**
 * @brief Class responsible for drawing on canvas
 */
class PaintingBrush {
public:

  /**
   * @brief Constructor
   * 
   * By default brushValue is set to 0.
   * By default brushSize is set to 1.
   * 
   * @param simulationSize 
   * @param simulationPadding 
   * @param framebufferSize 
   * @param canvasValueOffset 
   * @param canvasValueStride 
   * @return PaintingBrush 
   */
  static auto create(
    engine::Size2D<uint32_t> simulationSize,
    uint32_t simulationPadding,
    engine::Size2D<uint32_t> framebufferSize,
    uint32_t canvasValueOffset,
    uint32_t canvasValueStride
  ) -> PaintingBrush;

  PaintingBrush(const PaintingBrush&) = default;
  PaintingBrush(PaintingBrush&&) = default;

  auto operator=(const PaintingBrush&) -> PaintingBrush& = default;
  auto operator=(PaintingBrush&&) -> PaintingBrush& = default;

  auto paint(
    std::vector<uint8_t>& canvas,
    engine::Position2D<uint32_t> mousePosition
  ) const -> void;

  auto setValue(uint8_t value) -> void;
  auto setSize(uint8_t size) -> void;
  auto getSize() const -> uint8_t;
  auto setFramebufferSize(engine::Size2D<uint32_t> size) -> void;

private:
  PaintingBrush(
    engine::Size2D<uint32_t> simulationSize,
    uint32_t simulationPadding,
    engine::Size2D<uint32_t> framebufferSize,
    uint32_t canvasValueOffset,
    uint32_t canvasValueStride
  );

  // value that will be used to update canvas
  uint8_t m_brushValue;
  uint8_t m_brushSize;

  engine::Size2D<uint32_t> m_simulationSize;
  uint32_t m_simulationPadding;

  engine::Size2D<uint32_t> m_framebufferSize;

  uint32_t m_canvasValueOffset;
  uint32_t m_canvasValueStride;
};

#endif