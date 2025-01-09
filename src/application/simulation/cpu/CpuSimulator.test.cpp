#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/utils/position_mapping.hpp"
#include "application/simulation/cell.hpp"
#include "application/simulation/padding.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/dto/Size2D.hpp"
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
  uint8_t cell;
};

class TestFixture {
public:
  TestFixture()
    :canvasDescription({
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
    reset();
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
   * @brief Resets input and output to default state
   */
  auto reset() -> void {
    Position2D<uint32_t> position;
    for (position.y = 0; position.y < canvasDescription.size.height; ++position.y) {
      for (position.x = 0; position.x < canvasDescription.size.width; ++position.x) {
        const auto idx = mapSimulationPositionToCanvasIndex(position, canvasDescription);
        input[idx] = cell::PADDING;
        output[idx] = cell::PADDING;
      }
    }
  }

  /**
   * @brief Set expected value at position
   * 
   * @param position { .x = 0, .y = 0 } means centre of 5x5 grid
   * @param cell 
   */
  auto expect(Position2D<int32_t> position, uint8_t cell) -> void {
    expectations.emplace_back(Expectation {
      .position = position,
      .cell = cell
    });
  }

  auto runTest() -> void {
    const CpuSimulator simulator = *CpuSimulator::create({
      .width = canvasDescription.size.width - 2 * PADDING_SIZE,
      .height = canvasDescription.size.height - 2 * PADDING_SIZE
    });

    simulator.run(input, output);

    for (const auto [position, cell] : expectations) {
      const Position2D<uint32_t> absolutePosition = {
        .x = PADDING_SIZE + 2 + position.x,
        .y = PADDING_SIZE + 2 + position.y
      };
      const uint32_t idx = mapSimulationPositionToCanvasIndex(absolutePosition, canvasDescription);
      CHECK(output[idx] == cell);
    }
  }

private:
  PaintingCanvasDescription canvasDescription;

  vector<uint8_t> input;
  vector<uint8_t> output;
  vector<Expectation> expectations;
};

TEST_CASE_METHOD(TestFixture, "Air should not fall down through sand") {
  set({ 0, 1 }, cell::AIR);
  set({ 0, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::SAND);
  runTest();
}

TEST_CASE_METHOD(TestFixture, "Air should not fall down through water") {
  SECTION("water L") {
    set({ 0, 1 }, cell::AIR);
    set({ 0, 0 }, cell::WATER_L);
    expect({ 0, 1 }, cell::AIR);
    expect({ 0, 0 }, cell::WATER_L);
    runTest();
  }

  reset();

  SECTION("water R") {
    set({ 0, 1 }, cell::AIR);
    set({ 0, 0 }, cell::WATER_R);
    expect({ 0, 1 }, cell::AIR);
    expect({ 0, 0 }, cell::WATER_R);
    runTest();
  }
}

TEST_CASE_METHOD(TestFixture, "Sand should fall down through air") {
  set({ 0, 1 }, cell::SAND);
  set({ 0, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::SAND);
  runTest();
}

TEST_CASE_METHOD(TestFixture, "Sand should fall down through water") {
  SECTION("water L") {
    set({ 0, 1 }, cell::SAND);
    set({ 0, 0 }, cell::WATER_L);
    expect({ 0, 1 }, cell::WATER_L);
    expect({ 0, 0 }, cell::SAND);
    runTest();
  }

  reset();

  SECTION("water R") {
    set({ 0, 1 }, cell::SAND);
    set({ 0, 0 }, cell::WATER_R);
    expect({ 0, 1 }, cell::WATER_R);
    expect({ 0, 0 }, cell::SAND);
    runTest();
  }
}

TEST_CASE_METHOD(TestFixture, "Water should fall down through air") {
  SECTION("water L") {
    set({ 0, 1 }, cell::WATER_L);
    set({ 0, 0 }, cell::AIR);
    expect({ 0, 1 }, cell::AIR);
    expect({ 0, 0 }, cell::WATER_L);
    runTest();
  }

  reset();

  SECTION("water R") {
    set({ 0, 1 }, cell::WATER_R);
    set({ 0, 0 }, cell::AIR);
    expect({ 0, 1 }, cell::AIR);
    expect({ 0, 0 }, cell::WATER_R);
    runTest();
  }
}

TEST_CASE_METHOD(TestFixture, "Water should not fall down through sand") {
  SECTION("water L") {
    set({ 0, 1 }, cell::WATER_L);
    set({ 0, 0 }, cell::SAND);
    expect({ 0, 1 }, cell::WATER_L);
    expect({ 0, 0 }, cell::SAND);
    runTest();
  }

  reset();

  SECTION("water R") {
    set({ 0, 1 }, cell::WATER_R);
    set({ 0, 0 }, cell::SAND);
    expect({ 0, 1 }, cell::WATER_R);
    expect({ 0, 0 }, cell::SAND);
    runTest();
  }
}

TEST_CASE_METHOD(TestFixture, "Air should not fall diagonally through sand") {
  SECTION("left") {
    set({ 0, 1 }, cell::AIR);
    set({ -1, 0 }, cell::SAND);
    expect({ 0, 1 }, cell::AIR);
    expect({ -1, 0 }, cell::SAND);
    runTest();
  }

  reset();

  SECTION("right") {
    set({ 0, 1 }, cell::AIR);
    set({ 1, 0 }, cell::SAND);
    expect({ 0, 1 }, cell::AIR);
    expect({ 1, 0 }, cell::SAND);
    runTest();
  }
}

TEST_CASE_METHOD(TestFixture, "Air should not fall diagonally through water") {
  SECTION("left") {
    SECTION("water L") {
      set({ 0, 1 }, cell::AIR);
      set({ -1, 0, }, cell::WATER_L);
      expect({ 0, 1 }, cell::AIR);
      expect({ -1, 0, }, cell::WATER_L);
      runTest();
    }

    reset();

    SECTION("water R") {
      set({ 0, 1 }, cell::AIR);
      set({ -1, 0, }, cell::WATER_R);
      expect({ 0, 1 }, cell::AIR);
      expect({ -1, 0, }, cell::WATER_R);
      runTest();
    }
  }

  reset();

  SECTION("right") {
    SECTION("water L") {
      set({ 0, 1 }, cell::AIR);
      set({ 1, 0, }, cell::WATER_L);
      expect({ 0, 1 }, cell::AIR);
      expect({ 1, 0, }, cell::WATER_L);
      runTest();
    }

    reset();

    SECTION("water R") {
      set({ 0, 1 }, cell::AIR);
      set({ 1, 0, }, cell::WATER_R);
      expect({ 0, 1 }, cell::AIR);
      expect({ 1, 0, }, cell::WATER_R);
      runTest();
    }
  }
}

TEST_CASE_METHOD(TestFixture, "Sand should fall diagonally through air") {
  SECTION("left") {
    set({ 0, 1 }, cell::SAND);
    set({ -1, 0 }, cell::AIR);
    expect({ 0, 1 }, cell::AIR);
    expect({ -1, 0 }, cell::SAND);
    runTest();
  }

  reset();

  SECTION("right") {
    set({ 0, 1 }, cell::SAND);
    set({ 1, 0 }, cell::AIR);
    expect({ 0, 1 }, cell::AIR);
    expect({ 1, 0 }, cell::SAND);
    runTest();
  }
}

TEST_CASE_METHOD(TestFixture, "Sand should not fall diagonally through water") {
  SECTION("left") {
    SECTION("water L") {
      set({ 0, 1 }, cell::SAND);
      set({ -1, 0 }, cell::WATER_L);
      expect({ 0, 1 }, cell::SAND);
      expect({ -1, 0 }, cell::WATER_L);
      runTest();
    }

    reset();

    SECTION("water R") {
      set({ 0, 1 }, cell::SAND);
      set({ -1, 0 }, cell::WATER_R);
      expect({ 0, 1 }, cell::SAND);
      expect({ -1, 0 }, cell::WATER_R);
      runTest();
    }
  }

  reset();

  SECTION("right") {
    SECTION("water L") {
      set({ 0, 1 }, cell::SAND);
      set({ 1, 0 }, cell::WATER_L);
      expect({ 0, 1 }, cell::SAND);
      expect({ 1, 0 }, cell::WATER_L);
      runTest();
    }

    reset();

    SECTION("water R") {
      set({ 0, 1 }, cell::SAND);
      set({ 1, 0 }, cell::WATER_R);
      expect({ 0, 1 }, cell::SAND);
      expect({ 1, 0 }, cell::WATER_R);
      runTest();
    }
  }
}

TEST_CASE_METHOD(TestFixture, "Water should not fall diagonally through air") {
  SECTION("left") {
    SECTION("water L") {
      set({ 0, 1 }, cell::WATER_L);
      set({ -1, 0 }, cell::AIR);
      expect({ 0, 1 }, cell::WATER_L);
      expect({ -1, 0 }, cell::AIR);
      runTest();
    }

    reset();

    SECTION("water R") {
      set({ 0, 1 }, cell::WATER_R);
      set({ -1, 0 }, cell::AIR);
      expect({ 0, 1 }, cell::WATER_R);
      expect({ -1, 0 }, cell::AIR);
      runTest();
    }
  }

  reset();

  SECTION("right") {
    SECTION("water L") {
      set({ 0, 1 }, cell::WATER_L);
      set({ 1, 0 }, cell::AIR);
      expect({ 0, 1 }, cell::WATER_L);
      expect({ 1, 0 }, cell::AIR);
      runTest();
    }

    reset();

    SECTION("water R") {
      set({ 0, 1 }, cell::WATER_R);
      set({ 1, 0 }, cell::AIR);
      expect({ 0, 1 }, cell::WATER_R);
      expect({ 1, 0 }, cell::AIR);
      runTest();
    }
  }
}

TEST_CASE_METHOD(TestFixture, "Water should not fall diagonally through sand") {
  SECTION("left") {
    SECTION("water L") {
      set({ 0, 1 }, cell::WATER_L);
      set({ -1, 0 }, cell::SAND);
      expect({ 0, 1 }, cell::WATER_L);
      expect({ -1, 0 }, cell::SAND);
      runTest();
    }

    reset();

    SECTION("water R") {
      set({ 0, 1 }, cell::WATER_R);
      set({ -1, 0 }, cell::SAND);
      expect({ 0, 1 }, cell::WATER_R);
      expect({ -1, 0 }, cell::SAND);
      runTest();
    }
  }

  reset();

  SECTION("right") {
    SECTION("water L") {
      set({ 0, 1 }, cell::WATER_L);
      set({ 1, 0 }, cell::SAND);
      expect({ 0, 1 }, cell::WATER_L);
      expect({ 1, 0 }, cell::SAND);
      runTest();
    }

    reset();

    SECTION("water R") {
      set({ 0, 1 }, cell::WATER_R);
      set({ 1, 0 }, cell::SAND);
      expect({ 0, 1 }, cell::WATER_R);
      expect({ 1, 0 }, cell::SAND);
      runTest();
    }
  }
}

TEST_CASE_METHOD(TestFixture, "Sand should not move horizontaly through air") {
  SECTION("left") {
    set({ 0, 0 }, cell::SAND);
    set({ -1, 0 }, cell::AIR);
    expect({ 0, 0 }, cell::SAND);
    expect({ -1, 0 }, cell::AIR);
    runTest();
  }

  reset();

  SECTION("right") {
    set({ 0, 0 }, cell::SAND);
    set({ 1, 0 }, cell::AIR);
    expect({ 0, 0 }, cell::SAND);
    expect({ 1, 0 }, cell::AIR);
    runTest();
  }
}

TEST_CASE_METHOD(TestFixture, "Water should move horizontaly through air") {
  SECTION("left") {
    SECTION("water L") {
      set({ 0, 0 }, cell::WATER_L);
      set({ -1, 0 }, cell::AIR);
      expect({ 0, 0 }, cell::AIR);
      expect({ -1, 0 }, cell::WATER_L);
      runTest();
    }

    reset();

    SECTION("water R") {
      set({ 0, 0 }, cell::WATER_R);
      set({ -1, 0 }, cell::AIR);
      expect({ 0, 0 }, cell::AIR);
      expect({ -1, 0 }, cell::WATER_L); // After moving to left direction of water is changed
      runTest();
    }
  }

  reset();

  SECTION("right") {
    SECTION("water L") {
      set({ 0, 0 }, cell::WATER_L);
      set({ 1, 0 }, cell::AIR);
      expect({ 0, 0 }, cell::AIR);
      expect({ 1, 0 }, cell::WATER_R); // After moving to right direction of water is changed
      runTest();
    }

    reset();

    SECTION("water R") {
      set({ 0, 0 }, cell::WATER_R);
      set({ 1, 0 }, cell::AIR);
      expect({ 0, 0 }, cell::AIR);
      expect({ 1, 0 }, cell::WATER_R);
      runTest();
    }
  }
}

TEST_CASE_METHOD(TestFixture, "Water should not move horizontaly through sand") {
  SECTION("left") {
    SECTION("water L") {
      set({ 0, 0 }, cell::WATER_L);
      set({ -1, 0 }, cell::SAND);
      expect({ 0, 0 }, cell::WATER_L);
      expect({ -1, 0 }, cell::SAND);
      runTest();
    }

    reset();

    SECTION("water R") {
      set({ 0, 0 }, cell::WATER_R);
      set({ -1, 0 }, cell::SAND);
      expect({ 0, 0 }, cell::WATER_R);
      expect({ -1, 0 }, cell::SAND);
      runTest();
    }
  }

  reset();

  SECTION("right") {
    SECTION("water L") {
      set({ 0, 0 }, cell::WATER_L);
      set({ 1, 0 }, cell::SAND);
      expect({ 0, 0 }, cell::WATER_L);
      expect({ 1, 0 }, cell::SAND);
      runTest();
    }

    reset();

    SECTION("water R") {
      set({ 0, 0 }, cell::WATER_R);
      set({ 1, 0 }, cell::SAND);
      expect({ 0, 0 }, cell::WATER_R);
      expect({ 1, 0 }, cell::SAND);
      runTest();
    }
  }
}
