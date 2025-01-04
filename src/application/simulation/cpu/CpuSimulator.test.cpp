#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/painting/PaintingBrush.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/painting.hpp"
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
  auto paintingBrush = PaintingBrush::create(1, cell::PADDING);
  auto bufferIn = std::vector<uint8_t>(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride);
  auto bufferOut = std::vector<uint8_t>(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride);
  // TODO: fill bufferIn bufferOut

  const auto simulator = *CpuSimulator::create(size);

  const auto topPosition = Position2D<uint32_t>{ .x = 0 + PADDING_SIZE, .y = 1 + PADDING_SIZE };
  const auto botPosition = Position2D<uint32_t>{ .x = 0 + PADDING_SIZE, .y = 1 + PADDING_SIZE };
  const auto topIndex = mapPositionToCanvasIndex(topPosition, canvasDescription);
  const auto botIndex = mapPositionToCanvasIndex(botPosition, canvasDescription);

  SECTION("sand") {
    const auto fallThroughCells = std::array<uint8_t, 2> {
      cell::AIR,
      cell::WATER
    };

    for (const auto fallThroughCell : fallThroughCells) {
      paintingBrush.setValue(cell::SAND);
      paintingBrush.paint(bufferIn, canvasDescription, { .x = 0 + PADDING_SIZE, .y = 1 + PADDING_SIZE });
      paintingBrush.setValue(fallThroughCell);
      paintingBrush.paint(bufferIn, canvasDescription, { .x = 0 + PADDING_SIZE, .y = 0 + PADDING_SIZE });

      simulator.run(bufferIn, bufferOut);

      CHECK(bufferOut[topIndex] == fallThroughCell);
      CHECK(bufferOut[botIndex] == cell::SAND);
    }
  }
}