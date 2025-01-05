#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/simulation/padding.hpp"
#include "engine/utils/error.hpp"
#include <cassert>
#include <cmath>
#include <cstdint>

using std::vector;
using std::optional;
using engine::error;
using engine::Size2D;

static constexpr uint8_t FALL_DOWN_RULES[] = {
  0b00000000, // PADDING
  0b00000000, // AIR
  0b00001010, // SAND
  0b00000010  // WATER
};

static constexpr uint8_t FALL_DIAG_RULES[] = {
  0b00000000, // PADDING
  0b00000000, // AIR
  0b00000010, // SAND
  0b00000000  // WATER
};

auto CpuSimulator::create(Size2D<uint32_t> size) -> std::expected<CpuSimulator, engine::Error> {
  if (size.width < 1 || size.height < 1) {
    return std::unexpected(error("invalid simulation dimensions"));
  }

  const Size2D<uint32_t> sizeWithPadding = {
    .width = size.width + 2 * PADDING_SIZE,
    .height = size.height + 2 * PADDING_SIZE
  };

  return CpuSimulator(size, sizeWithPadding);
}

CpuSimulator::CpuSimulator(
  Size2D<uint32_t> size,
  Size2D<uint32_t> sizeWithPadding
)
  :m_size(size)
  ,m_sizeWithPadding(sizeWithPadding)
{}

auto CpuSimulator::run(
  const vector<uint8_t>& bufferIn,
  vector<uint8_t>& bufferOut
) const -> void {
  assert(bufferIn.size() == m_sizeWithPadding.width * m_sizeWithPadding.height);
  assert(bufferIn.size() == bufferOut.size());

  for (uint32_t row = 0; row < m_size.height; ++row) {
    for (uint32_t col = 0; col < m_size.width; ++col) {
      const uint32_t cellIdx = (row + PADDING_SIZE) * m_sizeWithPadding.width + col + PADDING_SIZE;

      if (const auto otherCell = fallStraight(bufferIn, cellIdx)) {
        // Can fall down
        if (!fallStraight(bufferIn, cellIdx - m_sizeWithPadding.width)) {
          // Cell below can't fall down
          bufferOut[cellIdx] = *otherCell;
          continue;
        }
      }

      // Can't fall down
      if (fallStraight(bufferIn, cellIdx + m_sizeWithPadding.width)) {
        // Cell above can fall here
        bufferOut[cellIdx] = bufferIn[cellIdx + m_sizeWithPadding.width];
        continue;
      }

      // If no rule applies to cell at cellIdx
      // just copy it to the bufferOut
      bufferOut[cellIdx] = bufferIn[cellIdx];
    }
  }
}

auto CpuSimulator::fallStraight(
  const vector<uint8_t>& bufferIn,
  uint32_t cellIdx
) const -> optional<uint8_t> {
  const uint8_t cell = bufferIn[cellIdx];
  const uint8_t cellRuleIdx = std::log2(cell);
  const uint8_t cellRule = FALL_DOWN_RULES[cellRuleIdx];

  const uint32_t otherIdx = cellIdx - m_sizeWithPadding.width;
  const uint8_t other = bufferIn[otherIdx];

  if ((other & cellRule) > 0) {
    return other;
  } else {
    return std::nullopt;
  }
}