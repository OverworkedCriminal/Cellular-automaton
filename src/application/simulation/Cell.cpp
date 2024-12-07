#include "application/simulation/Cell.hpp"
#include "engine/utils/error.hpp"

using engine::error;
using engine::input::KeyboardKey;

auto tryMapIntoCell(KeyboardKey key) -> std::expected<Cell, engine::Error> {
  switch (key) {
    case KeyboardKey::_1: return Cell::AIR;
    case KeyboardKey::_2: return Cell::SAND;
    case KeyboardKey::_3: return Cell::WATER;
    default:
      return std::unexpected(error("unsupported key"));
  }
}