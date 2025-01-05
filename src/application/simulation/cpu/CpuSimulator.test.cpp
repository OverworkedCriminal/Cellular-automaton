#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/brush/SquarePaintingBrush.hpp"
#include "application/painting/utils/position_mapping.hpp"
#include "application/simulation/cell.hpp"
#include "application/simulation/padding.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <vector>

using engine::Size2D;
using engine::Position2D;

TEST_CASE("Invalid width", "[constructor]") {
  const auto simulator = CpuSimulator::create({ .width = 0, .height = 1 });
  REQUIRE_FALSE(simulator.has_value());
}

TEST_CASE("Invalid height", "[constructor]") {
  const auto simulator = CpuSimulator::create({ .width = 1, .height = 0 });
  REQUIRE_FALSE(simulator.has_value());
}

TEST_CASE("Fall straight down", "[falling-down]") {
  const auto size = Size2D<uint32_t> {
    .width = 1,
    .height = 2
  };
  const auto canvasDescription = PaintingCanvasDescription {
    .size = {
      .width = size.width + 2 * PADDING_SIZE,
      .height = size.height + 2 * PADDING_SIZE
    },
    .paddingSize = PADDING_SIZE,
    .valueOffset = 0,
    .valueStride = 1
  };
  auto paintingBrush = SquarePaintingBrush::create();
  auto bufferIn = std::vector<uint8_t>(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride);
  auto bufferOut = std::vector<uint8_t>(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride);

  for (uint32_t row = 0; row < canvasDescription.size.height; ++row) {
    for (uint32_t col = 0; col < canvasDescription.size.width; ++col) {
      paintingBrush.paint(
        {
          .size = 1,
          .cell = cell::PADDING
        },
        {
          .x = col,
          .y = row
        },
        {
          .size = canvasDescription.size,
          .paddingSize = 0,
          .valueOffset = canvasDescription.valueOffset,
          .valueStride = canvasDescription.valueStride
        },
        bufferIn
      );
    }
  }

  const auto simulator = *CpuSimulator::create(size);

  const auto topPosition = Position2D<uint32_t>{ .x = 0 + PADDING_SIZE, .y = 1 + PADDING_SIZE };
  const auto botPosition = Position2D<uint32_t>{ .x = 0 + PADDING_SIZE, .y = 1 + PADDING_SIZE };
  const auto topIndex = mapSimulationPositionToCanvasIndex(topPosition, canvasDescription);
  const auto botIndex = mapSimulationPositionToCanvasIndex(botPosition, canvasDescription);

  SECTION("sand") {
    const auto fallThroughCells = std::array<uint8_t, 2> {
      cell::AIR,
      cell::WATER
    };

    for (const auto fallThroughCell : fallThroughCells) {
      paintingBrush.paint(
        {
          .size = 1,
          .cell = cell::SAND
        },
        {
          .x = 0 + PADDING_SIZE,
          .y = 1 + PADDING_SIZE
        },
        canvasDescription,
        bufferIn
      );

      paintingBrush.paint(
        {
          .size = 1,
          .cell = fallThroughCell
        },
        {
          .x = 0 + PADDING_SIZE,
          .y = 0 + PADDING_SIZE
        },
        canvasDescription,
        bufferIn
      );

      simulator.run(bufferIn, bufferOut);

      CHECK(bufferOut[topIndex] == fallThroughCell);
      CHECK(bufferOut[botIndex] == cell::SAND);
    }
  }
}