#ifndef APPLICATION_SIMULATION_TEST_CPU_SIMULATOR_FIXTURE_TEST_HPP
#define APPLICATION_SIMULATION_TEST_CPU_SIMULATOR_FIXTURE_TEST_HPP

#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/simulation/test/SimulatorFixture.test.hpp"
#include <stdexcept>

class TestFixture :public SimulatorFixture {
protected:
  CpuSimulator cpuSimulator;

  TestFixture()
    :cpuSimulator(createSimulator())
  {
    canvasDescription = {
      .size = {
        .width = 5 + 2 * PADDING_SIZE,
        .height = 5 + 2 * PADDING_SIZE
      },
      .paddingSize = PADDING_SIZE
    };

    input = std::vector<cell_t>(canvasDescription.size.width * canvasDescription.size.height, 0);
    output = std::vector<cell_t>(canvasDescription.size.width * canvasDescription.size.height, 0);

    resetBuffer(input);
    resetBuffer(output);
  }

  auto runSimulator() -> void override {
    cpuSimulator.run(input, output);
  }

private:
  auto createSimulator() -> CpuSimulator {
    auto cpuSimulatorResult = CpuSimulator::create({ .width = 5, .height = 5 }, CPU_SIMULATOR_CORE_COUNT);
    if (!cpuSimulatorResult.has_value()) {
      throw std::runtime_error("should create CpuSimulator");
    }

    return std::move(*cpuSimulatorResult);
  }
};

TEST_CASE("Invalid width", "[constructor]") {
  const auto simulator = CpuSimulator::create({ .width = 0, .height = 1 });
  REQUIRE_FALSE(simulator.has_value());
}

TEST_CASE("Invalid height", "[constructor]") {
  const auto simulator = CpuSimulator::create({ .width = 1, .height = 0 });
  REQUIRE_FALSE(simulator.has_value());
}

TEST_CASE("Invalid core count", "[constructor]") {
  const auto simulator = CpuSimulator::create({ .width = 1, .height = 1 }, 0);
  REQUIRE_FALSE(simulator.has_value());
}

#endif