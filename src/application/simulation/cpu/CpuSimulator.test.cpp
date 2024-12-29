#include "application/simulation/cpu/CpuSimulator.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Invalid width", "[constructor]") {
  auto simulator = CpuSimulator::create(0, 1);
  REQUIRE_FALSE(simulator.has_value());
}

TEST_CASE("Invalid height", "[constructor]") {
  auto simulator = CpuSimulator::create(1, 0);
  REQUIRE_FALSE(simulator.has_value());
}

TEST_CASE("Fall through air", "[falling-down]") {
  auto simulator = CpuSimulator::create(1, 2);

}