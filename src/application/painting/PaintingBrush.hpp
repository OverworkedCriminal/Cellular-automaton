#ifndef APPLICATION_PAINTING_PAINTING_BRUSH_HPP
#define APPLICATION_PAINTING_PAINTING_BRUSH_HPP

#include "application/painting/PaintingCanvasDescription.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include <cstdint>
#include <vector>

/**
 * @brief Class responsible for drawing on canvas
 */
class PaintingBrush {
public:
  static auto create(uint8_t size, uint8_t value) -> PaintingBrush;

  PaintingBrush(const PaintingBrush&) = default;
  PaintingBrush(PaintingBrush&&) = default;

  auto operator=(const PaintingBrush&) -> PaintingBrush& = default;
  auto operator=(PaintingBrush&&) -> PaintingBrush& = default;

  /**
   * @brief paint selected value onto canvas
   * 
   * @param canvas 
   * @param canvasDescription 
   * @param position (in simulation space)
   */
  auto paint(
    std::vector<uint8_t>& canvas,
    PaintingCanvasDescription& canvasDescription,
    engine::Position2D<uint32_t> position
  ) const -> void;

  auto setValue(uint8_t value) -> void;
  auto setSize(uint8_t size) -> void;
  auto getSize() const -> uint8_t;

private:
  PaintingBrush(uint8_t size, uint8_t value);

  /**
   * @brief value that will be used to update canvas
   */
  uint8_t m_brushValue;
  uint8_t m_brushSize;
};

#endif