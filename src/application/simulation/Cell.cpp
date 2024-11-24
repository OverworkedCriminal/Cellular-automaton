#include "application/simulation/Cell.hpp"
#include "engine/utils/error.hpp"

using engine::error;

auto tryMapIntoCell(engine::KeyboardKey key) -> std::expected<Cell, engine::Error> {
  switch (key) {
    case engine::KeyboardKey::_1: return Cell::AIR;
    case engine::KeyboardKey::_2: return Cell::SAND;
    case engine::KeyboardKey::_3: return Cell::WATER;
    default:
      return std::unexpected(error("unsupported key"));
  }
}