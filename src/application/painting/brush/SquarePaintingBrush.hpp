#ifndef APPLICATION_PAINTING_BRUSH_SQUARE_PAINTING_BRUSH_HPP
#define APPLICATION_PAINTING_BRUSH_SQUARE_PAINTING_BRUSH_HPP

#include "application/painting/brush/IPaintingBrush.hpp"

/**
 * @brief Class responsible for drawing squares on canvas
 */
class SquarePaintingBrush :public IPaintingBrush {
public:
  static auto create() -> SquarePaintingBrush;

  SquarePaintingBrush(const SquarePaintingBrush&) = default;
  SquarePaintingBrush(SquarePaintingBrush&&) = default;

  auto operator=(const SquarePaintingBrush&) -> SquarePaintingBrush& = default;
  auto operator=(SquarePaintingBrush&&) -> SquarePaintingBrush& = default;

  virtual auto paint(
    const PaintingBrushDescription brushDescription,
    const engine::Position2D<uint32_t> position,
    const PaintingCanvasDescription& canvasDescription,
    std::vector<cell_t>& canvas
  ) const -> void override;

private:
  SquarePaintingBrush();
};

#endif