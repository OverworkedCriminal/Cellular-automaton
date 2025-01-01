#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include "application/painting/painting.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/dto/Size2D.hpp"

using engine::Size2D;
using engine::Position2D;

TEST_CASE("mapping 100x100 -> 10x10", "[position-mapping]") {
  const Size2D<uint32_t> framebufferSize = {
    .width = 100,
    .height = 100
  };
  const Size2D<uint32_t> simulationSize = {
    .width = 10,
    .height = 10
  };

  for (uint32_t i = 0; i < 100; ++i) {
    Position2D<uint32_t> position = {
      .x = i,
      .y = i
    };

    auto mappedPosition = mapWindowPositionToSimulationPosition(
      position,
      framebufferSize,
      simulationSize
    );

    CHECK(mappedPosition.x == i / 10);
    CHECK(mappedPosition.y == i / 10);
  }
}

TEST_CASE("mapping 5x5 -> 2x2", "[position-mapping]") {
  const Size2D<uint32_t> framebufferSize = {
    .width = 10,
    .height = 10
  };
  const Size2D<uint32_t> simulationSize = {
    .width = 4,
    .height = 4
  };

  SECTION("x = 0, y = 0") {
    auto position = mapWindowPositionToSimulationPosition(
      {
        .x = 0,
        .y = 0
      },
      framebufferSize,
      simulationSize
    );
    CHECK(position.x == 0);
    CHECK(position.y == 0);
  };

  SECTION("x = 1, y = 1") {
    auto position = mapWindowPositionToSimulationPosition(
      {
        .x = 1,
        .y = 1
      },
      framebufferSize,
      simulationSize
    );
    CHECK(position.x == 0);
    CHECK(position.y == 0);
  }

  SECTION("x = 2, y = 2") {
    auto position = mapWindowPositionToSimulationPosition(
      {
        .x = 2,
        .y = 2
      },
      framebufferSize,
      simulationSize
    );
    CHECK(position.x == 0);
    CHECK(position.y == 0);
  }

  SECTION("x = 3, y = 3") {
    auto position = mapWindowPositionToSimulationPosition(
      {
        .x = 3,
        .y = 3
      },
      framebufferSize,
      simulationSize
    );
    CHECK(position.x == 1);
    CHECK(position.y == 1);
  }

  SECTION("x = 4, y = 4") {
    auto position = mapWindowPositionToSimulationPosition(
      {
        .x = 4,
        .y = 4
      },
      framebufferSize,
      simulationSize
    );
    CHECK(position.x == 1);
    CHECK(position.y == 1);
  }

}