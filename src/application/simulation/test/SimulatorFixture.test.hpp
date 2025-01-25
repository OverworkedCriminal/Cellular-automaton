#ifndef APPLICATION_SIMULATION_TEST_SIMULATOR_FIXTURE_TEST_HPP
#define APPLICATION_SIMULATION_TEST_SIMULATOR_FIXTURE_TEST_HPP

#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/utils/position_mapping.hpp"
#include "application/simulation/cell.hpp"
#include "application/simulation/padding.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include <bitset>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <vector>

struct Expectation {
  engine::Position2D<int32_t> position;
  uint8_t cellMask;
};

struct Error {
  engine::Position2D<int32_t> position;
  uint8_t cell;
  uint8_t expectedMask;
};

class SimulatorFixture {
protected:
  PaintingCanvasDescription canvasDescription;

  std::vector<uint8_t> input;
  std::vector<uint8_t> output;
  std::vector<Expectation> expectations;

  SimulatorFixture() {}
  virtual ~SimulatorFixture() {}

  /**
   * @brief Method should run simulator on input member and write its output to output member.
   */
  virtual auto runSimulator() -> void = 0;

  /**
   * @brief Set input value at position
   * 
   * @param position { .x = 0, .y = 0 } means centre of 5x5 grid
   * @param cell 
   */
  auto set(engine::Position2D<int32_t> position, uint8_t cell) -> void {
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
  auto expect(engine::Position2D<int32_t> position, uint8_t cellMask) -> void {
    expectations.push_back({
      .position = position,
      .cellMask = cellMask
    });
  }

  auto runTestExpectAnySuccess(uint32_t tries = 1) -> void {
    std::vector<std::vector<Error>> allErrors;

    for (uint32_t i = 0; i < tries; ++i) {
      std::vector<Error> errors = runTest();
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

      std::vector<Error> errors = runTest();
      CHECK(errors.empty());

      checkErrors(errors);
    }
  }

  auto runTest() -> std::vector<Error> {
    std::vector<Error> errors;

    resetBuffer(output);

    runSimulator();

    for (const auto& expectation : expectations) {
      const engine::Position2D<uint32_t> absolutePosition = {
        .x = PADDING_SIZE + 2 + expectation.position.x,
        .y = PADDING_SIZE + 2 + expectation.position.y
      };
      const uint32_t idx = mapSimulationPositionToCanvasIndex(absolutePosition, canvasDescription);
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

  auto resetBuffer(std::vector<uint8_t>& buffer) -> void {
    engine::Position2D<uint32_t> position;
    for (position.y = 0; position.y < canvasDescription.size.height; ++position.y) {
      for (position.x = 0; position.x < canvasDescription.size.width; ++position.x) {
        const auto idx = mapSimulationPositionToCanvasIndex(position, canvasDescription);
        buffer[idx] = cell::PADDING;
      }
    }
  }

  auto checkErrors(const std::vector<Error>& errors) -> void {
    for (const auto& [position, cell, expectedMask] : errors) {
      INFO("Position { x = " << position.x << ", y = " << position.y << " }");
      INFO("Expected mask " << std::bitset<8>(expectedMask));
      CHECK((cell & expectedMask) > 0);
    }
  }
};

#endif