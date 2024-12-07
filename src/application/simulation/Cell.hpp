#ifndef APPLICATION_SIMULATION_CELL_HPP
#define APPLICATION_SIMULATION_CELL_HPP

#include "engine/input/binding/KeyboardKey.hpp"
#include "engine/error/Error.hpp"
#include <cstdint>
#include <expected>

enum class Cell : uint8_t {
  PADDING = 1,
  AIR = 2,
  SAND = 4,
  WATER = 8
};

auto tryMapIntoCell(engine::input::KeyboardKey key) -> std::expected<Cell, engine::Error>;

#endif