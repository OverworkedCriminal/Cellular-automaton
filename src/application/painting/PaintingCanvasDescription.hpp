#ifndef APPLICATION_PAINTING_PAINTING_CANVAS_DESCRIPTION_HPP
#define APPLICATION_PAINTING_PAINTING_CANVAS_DESCRIPTION_HPP

#include "engine/utils/dto/Size2D.hpp"
#include <cstdint>

struct PaintingCanvasDescription {
  /**
   * @brief Size of the canvas (including padding)
   */
  engine::Size2D<uint32_t> size;

  /**
   * @brief Padding size. Padding is added to each side of the canvas
   */
  uint32_t paddingSize;

  /**
   * @brief Offset of the value within the element
   */
  uint32_t valueOffset;

  /**
   * @brief Distance to next element (in bytes)
   */
  uint32_t valueStride;
};

#endif