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
      CHECK(fallStraight(cell::SAND, cell::WATER_L));
      CHECK(fallStraight(cell::SAND, cell::WATER_R));
    }
  }

  SECTION("water") {
    SECTION("should not fall through padding") {
      CHECK_FALSE(fallStraight(cell::WATER_L, cell::PADDING));
      CHECK_FALSE(fallStraight(cell::WATER_R, cell::PADDING));
    }
    SECTION("should fall through air") {
      CHECK(fallStraight(cell::WATER_L, cell::AIR));
      CHECK(fallStraight(cell::WATER_R, cell::AIR));
    }
    SECTION("should not fall through sand") {
      CHECK_FALSE(fallStraight(cell::WATER_L, cell::SAND));
      CHECK_FALSE(fallStraight(cell::WATER_R, cell::SAND));
    }
    SECTION("should remain unchanged") {
      CHECK(fallStraight(cell::WATER_L, cell::WATER_L));
      CHECK_FALSE(fallStraight(cell::WATER_R, cell::WATER_L));
      CHECK_FALSE(fallStraight(cell::WATER_L, cell::WATER_R));
      CHECK(fallStraight(cell::WATER_R, cell::WATER_R));
    }
  }
}

class FallDiagTestFixture {
protected:
  TestFixture fixture = TestFixture::create({ .width = 3, .height = 2 });

  /**
   * @brief Parametrized test that checks falling diagonally down
   * 
   * @param topCell 
   * @param diagCell 
   * @param direction (-1 for left, 1 for right)
   * 
   * @return true when cells swapped positions
   * @return false when cells didn't swap position
   */
  auto fallDiag(uint8_t topCell, uint8_t diagCell, int8_t direction) -> bool {
    auto& [simulator, canvasDescription, bufferIn, bufferOut] = fixture;

    const Position2D<uint32_t> topPosition = { .x = 1 + PADDING_SIZE, .y = 1 + PADDING_SIZE };
    const Position2D<uint32_t> diagPosition = { .x = topPosition.x + direction, .y = 0 + PADDING_SIZE };
    const Position2D<uint32_t> otherDiagPosition = { .x = topPosition.x - direction, .y = 0 + PADDING_SIZE };
    const uint32_t topIndex = mapSimulationPositionToCanvasIndex(topPosition, canvasDescription);
    const uint32_t diagIndex = mapSimulationPositionToCanvasIndex(diagPosition, canvasDescription);
    const uint32_t otherDiagIndex = mapSimulationPositionToCanvasIndex(otherDiagPosition, canvasDescription);

    bufferIn[topIndex] = topCell;
    bufferIn[diagIndex] = diagCell;
    bufferIn[otherDiagIndex] = cell::PADDING;

    simulator.run(bufferIn, bufferOut);

    const bool topCorrect = bufferOut[topIndex] == diagCell;
    const bool diagCorrect = bufferOut[diagIndex] == topCell;

    return topCorrect && diagCorrect;
  }
};

TEST_CASE_METHOD(FallDiagTestFixture, "Fall diagonally down", "[fall-diag]") {
  SECTION("sand") {
    SECTION("should not fall through padding") {
      CHECK_FALSE(fallDiag(cell::SAND, cell::PADDING, -1));
      CHECK_FALSE(fallDiag(cell::SAND, cell::PADDING,  1));
    }
    SECTION("should fall through air") {
      CHECK(fallDiag(cell::SAND, cell::AIR, -1));
      CHECK(fallDiag(cell::SAND, cell::AIR,  1));
    }
    SECTION("should remain unchanged") {
      CHECK(fallDiag(cell::SAND, cell::SAND, -1));
      CHECK(fallDiag(cell::SAND, cell::SAND,  1));
    }
    SECTION("should not fall through water") {
      CHECK_FALSE(fallDiag(cell::SAND, cell::WATER_L, -1));
      CHECK_FALSE(fallDiag(cell::SAND, cell::WATER_L,  1));
      CHECK_FALSE(fallDiag(cell::SAND, cell::WATER_R, -1));
      CHECK_FALSE(fallDiag(cell::SAND, cell::WATER_R,  1));
    }
  }

  SECTION("water") {
    SECTION("should not fall through padding") {
      CHECK_FALSE(fallDiag(cell::WATER_L, cell::PADDING, -1));
      CHECK_FALSE(fallDiag(cell::WATER_L, cell::PADDING,  1));
      CHECK_FALSE(fallDiag(cell::WATER_R, cell::PADDING, -1));
      CHECK_FALSE(fallDiag(cell::WATER_R, cell::PADDING,  1));
    }
    SECTION("should not fall through air") {
      CHECK_FALSE(fallDiag(cell::WATER_L, cell::AIR, -1));
      CHECK_FALSE(fallDiag(cell::WATER_L, cell::AIR,  1));
      CHECK_FALSE(fallDiag(cell::WATER_R, cell::AIR, -1));
      CHECK_FALSE(fallDiag(cell::WATER_R, cell::AIR,  1));
    }
    SECTION("should not fall through sand") {
      CHECK_FALSE(fallDiag(cell::WATER_L, cell::SAND, -1));
      CHECK_FALSE(fallDiag(cell::WATER_L, cell::SAND,  1));
      CHECK_FALSE(fallDiag(cell::WATER_R, cell::SAND, -1));
      CHECK_FALSE(fallDiag(cell::WATER_R, cell::SAND,  1));
    }
    SECTION("should remain unchanged") {
      CHECK(fallDiag(cell::WATER_L, cell::WATER_L, -1));
      CHECK(fallDiag(cell::WATER_L, cell::WATER_L,  1));
      CHECK_FALSE(fallDiag(cell::WATER_L, cell::WATER_R, -1));
      CHECK_FALSE(fallDiag(cell::WATER_L, cell::WATER_R,  1));
      CHECK(fallDiag(cell::WATER_R, cell::WATER_R, -1));
      CHECK(fallDiag(cell::WATER_R, cell::WATER_R,  1));
      CHECK_FALSE(fallDiag(cell::WATER_R, cell::WATER_L, -1));
      CHECK_FALSE(fallDiag(cell::WATER_R, cell::WATER_L,  1));
    }
  }
}

class MoveHorizontalyTestFixture {
protected:
  TestFixture fixture = TestFixture::create({ .width = 3, .height = 1 });

  /**
   * @brief Parametrized test that checks moving horizontaly
   * 
   * @param centerCell 
   * @param sideCell 
   * @param direction (-1 for left, 1 for right)
   * 
   * @return true when cells swapped positions
   * @return false when cells didn't swap position
   */
  auto moveHorizontaly(uint8_t centerCell, uint8_t sideCell, int8_t direction) -> bool {
    auto& [simulator, canvasDescription, bufferIn, bufferOut] = fixture;

    const Position2D<uint32_t> centerPosition = { .x = 1 + PADDING_SIZE, .y = 0 + PADDING_SIZE };
    const Position2D<uint32_t> sidePosition = { .x = centerPosition.x + direction, .y = centerPosition.y };
    const Position2D<uint32_t> otherSidePosition = { .x = centerPosition.x - direction, .y = centerPosition.y };

    const uint32_t centerIndex = mapSimulationPositionToCanvasIndex(centerPosition, canvasDescription);
    const uint32_t sideIndex = mapSimulationPositionToCanvasIndex(sidePosition, canvasDescription);
    const uint32_t otherSideIndex = mapSimulationPositionToCanvasIndex(otherSidePosition, canvasDescription);

    bufferIn[centerIndex] = centerCell;
    bufferIn[sideIndex] = sideCell;
    bufferIn[otherSideIndex] = cell::PADDING;

    simulator.run(bufferIn, bufferOut);

    const bool topCorrect = bufferOut[centerIndex] == sideCell;
    const bool diagCorrect = bufferOut[sideIndex] == centerCell;

    return topCorrect && diagCorrect;
  }
};

TEST_CASE_METHOD(MoveHorizontalyTestFixture, "Move horizontaly", "[move-horizontaly]") {
  SECTION("sand") {
    SECTION("should not move through padding") {
      CHECK_FALSE(moveHorizontaly(cell::SAND, cell::PADDING, -1));
      CHECK_FALSE(moveHorizontaly(cell::SAND, cell::PADDING,  1));
    }
    SECTION("should not move through air") {
      CHECK_FALSE(moveHorizontaly(cell::SAND, cell::AIR, -1));
      CHECK_FALSE(moveHorizontaly(cell::SAND, cell::AIR,  1));
    }
    SECTION("should remain unchanged") {
      CHECK(moveHorizontaly(cell::SAND, cell::SAND, -1));
      CHECK(moveHorizontaly(cell::SAND, cell::SAND,  1));
    }
    SECTION("should not move through water") {
      CHECK_FALSE(moveHorizontaly(cell::SAND, cell::WATER_L, -1));
      CHECK_FALSE(moveHorizontaly(cell::SAND, cell::WATER_L,  1));
      CHECK_FALSE(moveHorizontaly(cell::SAND, cell::WATER_R, -1));
      CHECK_FALSE(moveHorizontaly(cell::SAND, cell::WATER_R,  1));
    }
  }

  SECTION("water") {
    SECTION("should not move through padding") {
      CHECK_FALSE(moveHorizontaly(cell::WATER_L, cell::PADDING, -1));
      CHECK_FALSE(moveHorizontaly(cell::WATER_L, cell::PADDING,  1));
      CHECK_FALSE(moveHorizontaly(cell::WATER_R, cell::PADDING, -1));
      CHECK_FALSE(moveHorizontaly(cell::WATER_R, cell::PADDING,  1));
    }
    SECTION("should move through air") {
      CHECK(moveHorizontaly(cell::WATER_L, cell::AIR, -1));
      CHECK(moveHorizontaly(cell::WATER_L, cell::AIR,  1));
      CHECK(moveHorizontaly(cell::WATER_R, cell::AIR, -1));
      CHECK(moveHorizontaly(cell::WATER_R, cell::AIR,  1));
    }
    SECTION("should not move through sand") {
      CHECK_FALSE(moveHorizontaly(cell::WATER_L, cell::SAND, -1));
      CHECK_FALSE(moveHorizontaly(cell::WATER_L, cell::SAND,  1));
      CHECK_FALSE(moveHorizontaly(cell::WATER_R, cell::SAND, -1));
      CHECK_FALSE(moveHorizontaly(cell::WATER_R, cell::SAND,  1));
    }
    SECTION("should remain unchanged") {
      CHECK(moveHorizontaly(cell::WATER_L, cell::WATER_L, -1));
      CHECK(moveHorizontaly(cell::WATER_L, cell::WATER_L,  1));
      CHECK_FALSE(moveHorizontaly(cell::WATER_L, cell::WATER_R, -1));
      CHECK_FALSE(moveHorizontaly(cell::WATER_L, cell::WATER_R,  1));
      CHECK(moveHorizontaly(cell::WATER_R, cell::WATER_R, -1));
      CHECK(moveHorizontaly(cell::WATER_R, cell::WATER_R,  1));
      CHECK_FALSE(moveHorizontaly(cell::WATER_R, cell::WATER_L, -1));
      CHECK_FALSE(moveHorizontaly(cell::WATER_R, cell::WATER_L,  1));
    }
  }
}