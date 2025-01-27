#ifndef APPLICATION_PAINTING_BRUSH_IPAINTING_BRUSH_HPP
#define APPLICATION_PAINTING_BRUSH_IPAINTING_BRUSH_HPP

#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/brush/PaintingBrushDescription.hpp"
#include "application/simulation/cell.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include <cstdint>
#include <vector>

class IPaintingBrush {
public:
  virtual ~IPaintingBrush() {}

  virtual auto paint(
    const PaintingBrushDescription brushDescription,
    const engine::Position2D<uint32_t> position,
    const PaintingCanvasDescription& canvasDescription,
    std::vector<cell_t>& canvas
  ) const -> void = 0;
};

#endif