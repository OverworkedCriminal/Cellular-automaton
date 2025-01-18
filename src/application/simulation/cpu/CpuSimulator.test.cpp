#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/utils/position_mapping.hpp"
#include "application/simulation/cell.hpp"
#include "application/simulation/padding.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <bitset>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
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

struct Expectation {
  Position2D<int32_t> position;
  uint8_t cellMask;
};

struct Error {
  Position2D<int32_t> position;
  uint8_t cell;
  uint8_t expectedMask;
};

class TestFixture {
public:
  TestFixture()
    :simulator(*CpuSimulator::create({
      .width = 5,
      .height = 5
    }))
    ,canvasDescription({
      .size = {
        .width = 5 + 2 * PADDING_SIZE,
        .height = 5 + 2 * PADDING_SIZE
      },
      .paddingSize = PADDING_SIZE,
      .valueOffset = 0,
      .valueStride = 1
    })
    ,input(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride, 0)
    ,output(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride, 0)
  {
    resetBuffer(input);
    resetBuffer(output);
  }

protected:
  /**
   * @brief Set input value at position
   * 
   * @param position { .x = 0, .y = 0 } means centre of 5x5 grid
   * @param cell 
   */
  auto set(Position2D<int32_t> position, uint8_t cell) -> void {
    const uint32_t idx = mapSimulationPositionToCanvasIndex(
      {
        .x = PADDING_SIZE + 2 + position.x,
        .y = PADDING_SIZE + 2 + position.y
      },
      canvasDescription
    );

    input[idx] = cell;
  }

  /**
   * @brief Set expected value at position
   * 
   * @param position { .x = 0, .y = 0 } means centre of 5x5 grid
   * @param cell 
   */
  auto expect(Position2D<int32_t> position, uint8_t cellMask) -> void {
    expectations.push_back({
      .position = position,
      .cellMask = cellMask
    });
  }

  auto runTestExpectAnySuccess(uint32_t tries = 1) -> void {
    vector<vector<Error>> allErrors;

    for (uint32_t i = 0; i < tries; ++i) {
      vector<Error> errors = runTest();
      if (errors.empty()) {
        SUCCEED("All expectations matched");
        return;
      }

      allErrors.emplace_back(std::move(errors));
    }

    for (uint32_t i = 0; i < tries; ++i) {
      INFO("Try " << i);
      checkErrors(allErrors[i]);
    }
  }

  auto runTestExpectAllSuccess(uint32_t tries = 1) -> void {
    for (uint32_t i = 0; i < tries; ++i) {
      INFO("Try " << i);

      vector<Error> errors = runTest();
      CHECK(errors.empty());

      checkErrors(errors);
    }
  }

private:
  CpuSimulator simulator;
  PaintingCanvasDescription canvasDescription;

  vector<uint8_t> input;
  vector<uint8_t> output;
  vector<Expectation> expectations;

  /**
   * @brief Resets input and output to default state
   */
  auto resetBuffer(std::vector<uint8_t>& buffer) -> void {
    Position2D<uint32_t> position;
    for (position.y = 0; position.y < canvasDescription.size.height; ++position.y) {
      for (position.x = 0; position.x < canvasDescription.size.width; ++position.x) {
        const auto idx = mapSimulationPositionToCanvasIndex(position, canvasDescription);
        buffer[idx] = cell::PADDING;
      }
    }
  }

  /**
   * @brief Maps relative position to bufferIndex
   * 
   * @param position 
   * @return uint32_t 
   */
  auto positionToIndex(Position2D<int32_t> position) -> uint32_t {
    const Position2D<uint32_t> absolutePosition = {
      .x = PADDING_SIZE + 2 + position.x,
      .y = PADDING_SIZE + 2 + position.y
    };
    const uint32_t idx = mapSimulationPositionToCanvasIndex(absolutePosition, canvasDescription);

    return idx;
  }

  auto runTest() -> vector<Error> {
    vector<Error> errors;

    resetBuffer(output);

    simulator.run(input, output);

    for (const auto& expectation : expectations) {
      const uint32_t idx = positionToIndex(expectation.position);
      if ((output[idx] & expectation.cellMask) == 0) {
        errors.push_back({
          .position = expectation.position,
          .cell = output[idx],
          .expectedMask = expectation.cellMask
        });
      }
    }

    return errors;
  }

  auto checkErrors(const vector<Error>& errors) -> void {
    for (const auto& [position, cell, expectedMask] : errors) {
      INFO("Position { x = " << position.x << ", y = " << position.y << " }");
      INFO("Expected mask " << std::bitset<8>(expectedMask));
      CHECK((cell & expectedMask) > 0);
    }
  }
};

TEST_CASE_METHOD(TestFixture, "AIR should not fall down through SAND") {
  set({ 0, 1 }, cell::AIR);
  set({ 0, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall down through WATER_L") {
  set({ 0, 1 }, cell::AIR);
  set({ 0, 0 }, cell::WATER_L);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_L | cell::WATER_R);
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall down through WATER_R") {
  set({ 0, 1 }, cell::AIR);
  set({ 0, 0 }, cell::WATER_R);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_R | cell::WATER_L);
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should fall down through AIR") {
  set({ 0, 1 }, cell::SAND);
  set({ 0, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "SAND should fall down through WATER_L") {
  set({ 0, 1 }, cell::SAND);
  set({ 0, 0 }, cell::WATER_L);
  expect({ 0, 1 }, cell::WATER_L);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "SAND should fall down through WATER_R") {
  set({ 0, 1 }, cell::SAND);
  set({ 0, 0 }, cell::WATER_R);
  expect({ 0, 1 }, cell::WATER_R);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "WATER_L should fall down through AIR") {
  set({ 0, 1 }, cell::WATER_L);
  set({ 0, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_L);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "WATER_R should fall down through AIR") {
  set({ 0, 1 }, cell::WATER_R);
  set({ 0, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_R);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "WATER_L should not fall down through SAND") {
  set({ 0, 1 }, cell::WATER_L);
  set({ 0, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_L | cell::WATER_R);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should not fall down through SAND") {
  set({ 0, 1 }, cell::WATER_R);
  set({ 0, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_R | cell::WATER_L);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally left through SAND") {
  set({ 0, 1 }, cell::AIR);
  set({ -1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0 }, cell::SAND);

  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally right through SAND") {
  set({ 0, 1 }, cell::AIR);
  set({ 1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0 }, cell::SAND);

  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally left through WATER_L") {
  set({ 0, 1 }, cell::AIR);
  set({ -1, 0, }, cell::WATER_L);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0, }, cell::WATER_L | cell::WATER_R);

  // run twice because moving diagonally happens in one direction at the time
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally left through WATER_R") {
  set({ 0, 1 }, cell::AIR);
  set({ -1, 0, }, cell::WATER_R);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0, }, cell::WATER_R | cell::WATER_L);

  // run twice because moving diagonally happens in one direction at the time
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally right through WATER_L") {
  set({ 0, 1 }, cell::AIR);
  set({ 1, 0, }, cell::WATER_L);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0, }, cell::WATER_L | cell::WATER_R);

  // run test twice because moving diagonally happens in one direction at the time
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally right through WATER_R") {
  set({ 0, 1 }, cell::AIR);
  set({ 1, 0, }, cell::WATER_R);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0, }, cell::WATER_R | cell::WATER_L);

  // run test twice because moving diagonally happens in one direction at the time
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should fall diagonally left through AIR") {
  set({ 0, 1 }, cell::SAND);
  set({ -1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0 }, cell::SAND);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should fall diagonally right through AIR") {
  set({ 0, 1 }, cell::SAND);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0 }, cell::SAND);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not fall diagonally left through WATER_L") {
  set({  0, 1 }, cell::SAND);
  set({ -1, 0 }, cell::WATER_L);
  expect({  0, 1 }, cell::SAND);
  expect({ -1, 0 }, cell::WATER_L | cell::WATER_R);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not fall diagonally left through WATER_R") {
  set({  0, 1 }, cell::SAND);
  set({ -1, 0 }, cell::WATER_R);
  expect({  0, 1 }, cell::SAND);
  expect({ -1, 0 }, cell::WATER_R | cell::WATER_L);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not fall diagonally right through WATER_L") {
  set({ 0, 1 }, cell::SAND);
  set({ 1, 0 }, cell::WATER_L);
  expect({ 0, 1 }, cell::SAND);
  expect({ 1, 0 }, cell::WATER_L | cell::WATER_R);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not fall diagonally right through WATER_R") {
  set({ 0, 1 }, cell::SAND);
  set({ 1, 0 }, cell::WATER_R);
  expect({ 0, 1 }, cell::SAND);
  expect({ 1, 0 }, cell::WATER_R | cell::WATER_L);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should fall diagonally left through AIR") {
  set({ 0, 1 }, cell::WATER_L);
  set({ -1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should fall diagonally right through AIR") {
  set({ 0, 1 }, cell::WATER_L);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should fall diagonally left through AIR") {
  set({ 0, 1 }, cell::WATER_R);
  set({ -1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0 }, cell::WATER_R);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should fall diagonally right through AIR") {
  set({ 0, 1 }, cell::WATER_R);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0 }, cell::WATER_R);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should not fall diagonally left through SAND") {
  set({ 0, 1 }, cell::WATER_L);
  set({ -1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_L | cell::WATER_R);
  expect({ -1, 0 }, cell::SAND);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should not fall diagonally right through SAND") {
  set({ 0, 1 }, cell::WATER_L);
  set({ 1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_L | cell::WATER_R);
  expect({ 1, 0 }, cell::SAND);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should not fall diagonally left through SAND") {
  set({ 0, 1 }, cell::WATER_R);
  set({ -1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_R | cell::WATER_L);
  expect({ -1, 0 }, cell::SAND);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should not fall diagonally right through SAND") {
  set({ 0, 1 }, cell::WATER_R);
  set({ 1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_R | cell::WATER_L);
  expect({ 1, 0 }, cell::SAND);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not move horizontally left through AIR") {
  set({  0, 0 }, cell::SAND);
  set({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::SAND);
  expect({ -1, 0 }, cell::AIR);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not move horizontally right through AIR") {
  set({ 0, 0 }, cell::SAND);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 0 }, cell::SAND);
  expect({ 1, 0 }, cell::AIR);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should move horizontally left through AIR") {
  set({  0, 0 }, cell::WATER_L);
  set({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::AIR);
  expect({ -1, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L when can't move horizontally left should change to WATER_R") {
  set({ 0, 0 }, cell::WATER_L);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_R);
  expect({ 1, 0 }, cell::AIR);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R when can't move horizontally right should change to WATER_L") {
  set({ -1, 0 }, cell::AIR);
  set({  0, 0 }, cell::WATER_R);
  expect({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should move horizontally right through AIR") {
  set({ 0, 0 }, cell::WATER_R);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 0 }, cell::AIR);
  expect({ 1, 0 }, cell::WATER_R);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should not move horizontally left through SAND and should not move right through AIR but should change to WATER_R") {
  set({ -1, 0 }, cell::SAND);
  set({  0, 0 }, cell::WATER_L);
  set({  1, 0 }, cell::AIR);
  expect({ -1, 0 }, cell::SAND);
  expect({  0, 0 }, cell::WATER_R);
  expect({  1, 0 }, cell::AIR);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should not move horizontally right through SAND and should not move left through AIR but should change to WATER_L") {
  set({ -1, 0 }, cell::AIR);
  set({  0, 0 }, cell::WATER_R);
  set({  1, 0 }, cell::SAND);
  expect({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::WATER_L);
  expect({  1, 0 }, cell::SAND);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L and WATER_R race for center position") {
  set({ -1, 0 }, cell::WATER_R);
  set({  0, 0 }, cell::AIR);
  set({  1, 0 }, cell::WATER_L);
  expect({ -1, 0 }, cell::WATER_R);
  expect({  0, 0 }, cell::WATER_L);
  expect({  1, 0 }, cell::AIR);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R and WATER_L race for center position") {
  set({ -1, 0 }, cell::WATER_R);
  set({  0, 0 }, cell::AIR);
  set({  1, 0 }, cell::WATER_L);
  expect({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::WATER_R);
  expect({  1, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should move horizontally through WATER_R") {
  set({ -1, 0 }, cell::WATER_R);
  set({  0, 0 }, cell::WATER_L);
  expect({ -1, 0 }, cell::WATER_L);
  expect({  0, 0 }, cell::WATER_R);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should move horizontally through WATER_L") {
  set({ 0, 0 }, cell::WATER_R);
  set({ 1, 0 }, cell::WATER_L);
  set({ 0, 0 }, cell::WATER_L);
  set({ 1, 0 }, cell::WATER_R);

  runTestExpectAllSuccess(2);
}
