#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/simulation/padding.hpp"
#include "engine/utils/error.hpp"
#include <cassert>
#include <cmath>
#include <cstdint>

using engine::error;
using engine::Size2D;

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
  const std::vector<uint8_t>& bufferIn,
  std::vector<uint8_t>& bufferOut
) const -> void {
  assert(bufferIn.size() == m_sizeWithPadding.width * m_sizeWithPadding.height);
  assert(bufferIn.size() == bufferOut.size());

  constexpr uint8_t FALL_RULES[] {
    0b00000000, // PADDING
    0b00000000, // AIR
    0b00001010, // SAND
    0b00000010  // WATER
  };

  for (uint32_t row = 0; row < m_size.height; ++row) {
    for (uint32_t col = 0; col < m_size.width; ++col) {
      const uint32_t cellIdx = (row + PADDING_SIZE) * m_sizeWithPadding.width + col + PADDING_SIZE;
      uint32_t otherIdx;
      uint8_t other;

      const uint8_t cell = bufferIn[cellIdx];

      // Fall down
      otherIdx = cellIdx + m_sizeWithPadding.width;
      other = bufferIn[otherIdx];
      const uint32_t otherRuleIdx = std::log2(other);
      const uint8_t otherRule = FALL_RULES[otherRuleIdx];
      if ((cell & otherRule) > 0) {
        // Other can fall here
        bufferOut[cellIdx] = bufferIn[otherIdx];
        continue;
      }

      const uint32_t cellRuleIdx = std::log2(cell);
      const uint8_t cellRule = FALL_RULES[cellRuleIdx];

      otherIdx = cellIdx - m_sizeWithPadding.width;
      other = bufferIn[otherIdx];
      if ((other & cellRule) > 0) {
        // Can fall down
        const uint32_t otherRuleIdx = std::log2(other);
        const uint8_t otherRule = FALL_RULES[otherRuleIdx];

        const uint32_t otherDownIdx = otherIdx - m_sizeWithPadding.width;
        const uint8_t otherDown = bufferIn[otherDownIdx];
        if ((otherDown & otherRule) == 0) {
          // Other can't fall down
          bufferOut[cellIdx] = bufferIn[otherIdx];
          continue;
        }
      }

      bufferOut[cellIdx] = bufferIn[cellIdx];
    }
  }
}