#include "application/painting/brush/PaintingBrushDescription.hpp"
#include "application/painting/brush/SquarePaintingBrush.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>

constexpr uint8_t EMPTY_VALUE = 0;
constexpr uint8_t BRUSH_VALUE = 1;

const SquarePaintingBrush paintingBrush = SquarePaintingBrush::create();
const PaintingBrushDescription brushDescription = {
  .size = 1,
  .cell = BRUSH_VALUE
};

TEST_CASE("painting 0-padding 0-offset 1-stride", "[painting]") {
  const PaintingCanvasDescription canvasDescription = {
    .size = {
      .width = 2,
      .height = 2
    },
    .paddingSize = 0,
    .valueOffset = 0,
    .valueStride = 1
  };
  std::vector<uint8_t> canvas(canvasDescription.size.width * canvasDescription.size.height);
  
  SECTION("position (0, 0)") {
    std::ranges::fill(canvas, EMPTY_VALUE);
    paintingBrush.paint(brushDescription, { .x = 0, .y = 0 }, canvasDescription, canvas);
    CHECK(canvas[0] == BRUSH_VALUE);
    CHECK(canvas[1] == EMPTY_VALUE);
    CHECK(canvas[2] == EMPTY_VALUE);
    CHECK(canvas[3] == EMPTY_VALUE);
  }

  SECTION("position (1, 0)") {
    std::ranges::fill(canvas, EMPTY_VALUE);
    paintingBrush.paint(brushDescription, { .x = 1, .y = 0 }, canvasDescription, canvas);
    CHECK(canvas[0] == EMPTY_VALUE);
    CHECK(canvas[1] == BRUSH_VALUE);
    CHECK(canvas[2] == EMPTY_VALUE);
    CHECK(canvas[3] == EMPTY_VALUE);
  }

  SECTION("position (0, 1)") {
    std::ranges::fill(canvas, EMPTY_VALUE);
    paintingBrush.paint(brushDescription, { .x = 0, .y = 1 }, canvasDescription, canvas);
    CHECK(canvas[0] == EMPTY_VALUE);
    CHECK(canvas[1] == EMPTY_VALUE);
    CHECK(canvas[2] == BRUSH_VALUE);
    CHECK(canvas[3] == EMPTY_VALUE);
  }

  SECTION("position (1, 1)") {
    std::ranges::fill(canvas, EMPTY_VALUE);
    paintingBrush.paint(brushDescription, { .x = 1, .y = 1 }, canvasDescription, canvas);
    CHECK(canvas[0] == EMPTY_VALUE);
    CHECK(canvas[1] == EMPTY_VALUE);
    CHECK(canvas[2] == EMPTY_VALUE);
    CHECK(canvas[3] == BRUSH_VALUE);
  }
}

TEST_CASE("painting 2-padding 0-offset 1-stride", "[painting]") {
  constexpr uint32_t PADDING_SIZE = 2;
  const PaintingCanvasDescription canvasDescription = {
    .size = {
      .width = 1 + 2 * PADDING_SIZE,
      .height = 1 + 2 * PADDING_SIZE
    },
    .paddingSize = PADDING_SIZE,
    .valueOffset = 0,
    .valueStride = 1
  };
  std::vector<uint8_t> canvas(canvasDescription.size.width * canvasDescription.size.height);

  const auto [width, height] = canvasDescription.size;

  SECTION("painting on padding") {
    for (uint32_t row = 0; row < height; ++row) {
      for (uint32_t col = 0; col < width; ++col) {
        if (
          row >= PADDING_SIZE &&
          row < height - PADDING_SIZE &&
          col >= PADDING_SIZE &&
          col < width - PADDING_SIZE
        ) {
          continue;
        }

        std::ranges::fill(canvas, EMPTY_VALUE);
        paintingBrush.paint(brushDescription, { .x = col, .y = row }, canvasDescription, canvas);
        const bool canvasUnchanged = std::ranges::all_of(canvas, [](auto value) { return value == EMPTY_VALUE; });
        CHECK(canvasUnchanged);
      }
    }
  }

  SECTION("position (2, 2)") {
    constexpr uint32_t row = 0 + PADDING_SIZE;
    constexpr uint32_t col = 0 + PADDING_SIZE;

    std::ranges::fill(canvas, EMPTY_VALUE);
    paintingBrush.paint(brushDescription, { .x = col, .y = row }, canvasDescription, canvas);

    uint32_t idx = row * width + col;
    CHECK(canvas[idx] == BRUSH_VALUE);
  }
}

TEST_CASE("painting 0-padding 0-offset 4-stride", "[painting]") {
  const PaintingCanvasDescription canvasDescription = {
    .size = {
      .width = 1,
      .height = 1
    },
    .paddingSize = 0,
    .valueOffset = 0,
    .valueStride = 4
  };
  std::vector<uint8_t> canvas(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride);

  SECTION("position (0, 0)") {
    std::ranges::fill(canvas, EMPTY_VALUE);
    paintingBrush.paint(brushDescription, { .x = 0, .y = 0 }, canvasDescription, canvas);

    CHECK(canvas[0] == BRUSH_VALUE);
    for (uint32_t i = 1; i < canvas.size(); ++i) {
      CHECK(canvas[i] == EMPTY_VALUE);
    }
  }
}

TEST_CASE("painting 0-padding 3-offset 4-stride", "[painting]") {
  const PaintingCanvasDescription canvasDescription = {
    .size = {
      .width = 1,
      .height = 1
    },
    .paddingSize = 0,
    .valueOffset = 3,
    .valueStride = 4
  };
  std::vector<uint8_t> canvas(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride);

  SECTION("position (0, 0)") {
    std::ranges::fill(canvas, EMPTY_VALUE);
    paintingBrush.paint(brushDescription, { .x = 0, .y = 0 }, canvasDescription, canvas);

    CHECK(canvas[3] == BRUSH_VALUE);
    for (uint32_t i = 0; i < canvas.size() - 1; ++i) {
      CHECK(canvas[i] == EMPTY_VALUE);
    }
  }
}