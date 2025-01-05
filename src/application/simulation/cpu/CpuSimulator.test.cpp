#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/utils/position_mapping.hpp"
#include "application/simulation/cell.hpp"
#include "application/simulation/padding.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <catch2/catch_test_macros.hpp>
#include <vector>

using std::vector;
using engine::Position2D;
using engine::Size2D;

TEST_CASE("Invalid width", "[constructor]") {
  const auto simulator = CpuSimulator::create({ .width = 0, .height = 1 });
  REQUIRE_FALSE(simulator.has_value());
}

TEST_CASE("Invalid height", "[constructor]") {
  const auto simulator = CpuSimulator::create({ .width = 1, .height = 0 });
  REQUIRE_FALSE(simulator.has_value());
}

struct TestFixture {
  CpuSimulator simulator;
  PaintingCanvasDescription canvasDescription;
  vector<uint8_t> bufferIn;
  vector<uint8_t> bufferOut;

  static auto create(Size2D<uint32_t> simulationSize) -> TestFixture {
    CpuSimulator simulator = *CpuSimulator::create(simulationSize);
    PaintingCanvasDescription canvasDescription = {
      .size = {
        .width = simulationSize.width + 2 * PADDING_SIZE,
        .height = simulationSize.height + 2 * PADDING_SIZE
      },
      .paddingSize = PADDING_SIZE,
      .valueOffset = 0,
      .valueStride = 1
    };
    vector<uint8_t> bufferIn(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride, 0);
    vector<uint8_t> bufferOut(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride, 0);

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

    return {
      .simulator = std::move(simulator),
      .canvasDescription = std::move(canvasDescription),
      .bufferIn = std::move(bufferIn),
      .bufferOut = std::move(bufferOut)
    };
  }
};

class FallStraightFixture {
protected:
  TestFixture fixture = TestFixture::create({ .width = 1, .height = 2 });

  /**
   * @brief Parametrized test that checks falling straight down
   * 
   * @param topCell 
   * @param botCell 
   * 
   * @return true when cells swapped positions
   * @return false when cells didn't swap position
   */
  auto fallStraight(uint8_t topCell, uint8_t botCell) -> bool {
    auto& [simulator, canvasDescription, bufferIn, bufferOut] = fixture;

    const Position2D<uint32_t> topPosition = { .x = 0 + PADDING_SIZE, .y = 1 + PADDING_SIZE };
    const Position2D<uint32_t> botPosition = { .x = 0 + PADDING_SIZE, .y = 0 + PADDING_SIZE };
    const uint32_t topIndex = mapSimulationPositionToCanvasIndex(topPosition, canvasDescription);
    const uint32_t botIndex = mapSimulationPositionToCanvasIndex(botPosition, canvasDescription);

    bufferIn[topIndex] = topCell;
    bufferIn[botIndex] = botCell;

    simulator.run(bufferIn, bufferOut);

    const bool topCorrect = bufferOut[topIndex] == botCell;
    const bool botCorrect = bufferOut[botIndex] == topCell;

    return topCorrect && botCorrect;
  }
};

TEST_CASE_METHOD(FallStraightFixture, "Fall straight down", "[fall-down]") {
  SECTION("sand") {
    SECTION("should not fall through padding") {
      CHECK_FALSE(fallStraight(cell::SAND, cell::PADDING));
    }
    SECTION("should fall through air") {
      CHECK(fallStraight(cell::SAND, cell::AIR));
    }
    SECTION("should remain unchanged") {
      CHECK(fallStraight(cell::SAND, cell::SAND));
    }
    SECTION("should fall through water") {
      CHECK(fallStraight(cell::SAND, cell::WATER));
    }
  }

  SECTION("water") {
    SECTION("should not fall through padding") {
      CHECK_FALSE(fallStraight(cell::WATER, cell::PADDING));
    }
    SECTION("should fall through air") {
      CHECK(fallStraight(cell::WATER, cell::AIR));
    }
    SECTION("should not fall through sand") {
      CHECK_FALSE(fallStraight(cell::WATER, cell::SAND));
    }
    SECTION("should remain unchanged") {
      CHECK(fallStraight(cell::WATER, cell::WATER));
    }
  }
}
