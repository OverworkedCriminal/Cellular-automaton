#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/utils/position_mapping.hpp"
#include "application/simulation/cell.hpp"
#include "application/simulation/padding.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include <catch2/catch_test_macros.hpp>
#include <vector>

using std::vector;
using engine::Position2D;

TEST_CASE("Invalid width", "[constructor]") {
  const auto simulator = CpuSimulator::create({ .width = 0, .height = 1 });
  REQUIRE_FALSE(simulator.has_value());
}

TEST_CASE("Invalid height", "[constructor]") {
  const auto simulator = CpuSimulator::create({ .width = 1, .height = 0 });
  REQUIRE_FALSE(simulator.has_value());
}

class FallStraightDownFixture {
public:
  FallStraightDownFixture()
    :simulator(*CpuSimulator::create({ .width = 1, .height = 2 }))
    ,canvasDescription({
      .size = {
        .width = 1 + 2 * PADDING_SIZE,
        .height = 2 + 2 * PADDING_SIZE
      },
      .paddingSize = PADDING_SIZE,
      .valueOffset = 0,
      .valueStride = 1
    })
    ,bufferIn(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride, 0)
    ,bufferOut(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride, 0)
  {
    for (uint32_t row = 0; row < canvasDescription.size.height; ++row) {
      for (uint32_t col = 0; col < canvasDescription.size.width; ++col) {
        const uint32_t idx = mapSimulationPositionToCanvasIndex(
          {
            .x = col,
            .y = row
          },
          canvasDescription
        );
        bufferIn[idx] = cell::PADDING;
        bufferOut[idx] = cell::PADDING;
      }
    }
  }

protected:
  CpuSimulator simulator;
  PaintingCanvasDescription canvasDescription;
  vector<uint8_t> bufferIn;
  vector<uint8_t> bufferOut;

  auto testFallStraightDown(uint8_t topCell, uint8_t botCell) -> void {
    const Position2D<uint32_t> topPosition = { .x = 0 + PADDING_SIZE, .y = 1 + PADDING_SIZE };
    const Position2D<uint32_t> botPosition = { .x = 0 + PADDING_SIZE, .y = 0 + PADDING_SIZE };
    const uint32_t topIndex = mapSimulationPositionToCanvasIndex(topPosition, canvasDescription);
    const uint32_t botIndex = mapSimulationPositionToCanvasIndex(botPosition, canvasDescription);

    bufferIn[topIndex] = topCell;
    bufferIn[botIndex] = botCell;

    simulator.run(bufferIn, bufferOut);

    CHECK(bufferOut[topIndex] == botCell);
    CHECK(bufferOut[botIndex] == topCell);
  }
};

TEST_CASE_METHOD(FallStraightDownFixture, "Fall straight down", "[fall-down]") {
  SECTION("sand through air") {
    testFallStraightDown(cell::SAND, cell::AIR);
  }
  SECTION("sand through water") {
    testFallStraightDown(cell::SAND, cell::WATER);
  }
}
