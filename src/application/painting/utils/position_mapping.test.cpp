#include "application/painting/utils/position_mapping.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include "application/painting/PaintingCanvasDescription.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/dto/Size2D.hpp"

using engine::Size2D;
using engine::Position2D;

TEST_CASE("mapWindowPositionToSimulationPosition 100x100 -> 10x10", "[position-mapping]") {
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

TEST_CASE("mapWindowPositionToSimulationPosition 5x5 -> 2x2", "[position-mapping]") {
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

TEST_CASE("mapSimulationPositionToCanvasIndex 10x10 0-offset 1-stride") {
  const PaintingCanvasDescription canvasDescription = {
    .size = {
      .width = 10,
      .height = 10
    },
    .paddingSize = 0,
    .valueOffset = 0,
    .valueStride = 1
  };

  SECTION("x=0, y=0 should return 0") {
    const uint32_t idx = mapSimulationPositionToCanvasIndex({ .x = 0, .y = 0 }, canvasDescription);
    CHECK(idx == 0);
  }

  SECTION("x=5, y=0 should return 5") {
    const uint32_t idx = mapSimulationPositionToCanvasIndex({ .x = 5, .y = 0 }, canvasDescription);
    CHECK(idx == 5);
  }

  SECTION("x=2, y=3 should return 32") {
    const uint32_t idx = mapSimulationPositionToCanvasIndex({ .x = 2, .y = 3 }, canvasDescription);
    CHECK(idx == 32);
  }
}

TEST_CASE("mapSimulationPositionToCanvasIndex 5x5 0-offset 4-stride") {
  const PaintingCanvasDescription canvasDescription = {
    .size = {
      .width = 5,
      .height = 5
    },
    .paddingSize = 0,
    .valueOffset = 0,
    .valueStride = 4
  };

  SECTION("x=0, y=0 should return 0") {
    const uint32_t idx = mapSimulationPositionToCanvasIndex({ .x = 0, .y = 0 }, canvasDescription);
    CHECK(idx == 0);
  }

  SECTION("x=2, y=0 should return 2") {
    const uint32_t idx = mapSimulationPositionToCanvasIndex({ .x = 2, .y = 0 }, canvasDescription);
    CHECK(idx == 8);
  }

  SECTION("x=1, y=2 should return 44") {
    const uint32_t idx = mapSimulationPositionToCanvasIndex({ .x = 1, .y = 2 }, canvasDescription);
    CHECK(idx == 44);
  }
}

TEST_CASE("mapSimulationPositionToCanvasIndex 4x4 3-offset 4-stride") {
  const PaintingCanvasDescription canvasDescription = {
    .size = {
      .width = 4,
      .height = 4
    },
    .paddingSize = 0,
    .valueOffset = 3,
    .valueStride = 4
  };

  SECTION("x=0, y=0 should return 3") {
    const uint32_t idx = mapSimulationPositionToCanvasIndex({ .x = 0, .y = 0 }, canvasDescription);
    CHECK(idx == 3);
  }

  SECTION("x=3, y=0 should return 15") {
    const uint32_t idx = mapSimulationPositionToCanvasIndex({ .x = 3, .y = 0 }, canvasDescription);
    CHECK(idx == 15);
  }

  SECTION("x=2, y=3 should return 59") {
    const uint32_t idx = mapSimulationPositionToCanvasIndex({ .x = 2, .y = 3 }, canvasDescription);
    CHECK(idx == 59);
  }
}