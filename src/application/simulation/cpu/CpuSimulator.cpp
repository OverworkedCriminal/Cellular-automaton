#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/simulation/padding.hpp"
#include "engine/utils/error.hpp"
#include <cassert>
#include <cmath>

using engine::error;

auto CpuSimulator::create(
  uint32_t width,
  uint32_t height
) -> std::expected<CpuSimulator, engine::Error> {
  if (width < 1 || height < 1) {
    return std::unexpected(error("invalid simulation dimensions"));
  }

  return CpuSimulator(
    width,
    height,
    width + 2 * PADDING_SIZE,
    height + 2 * PADDING_SIZE
  );
}

CpuSimulator::CpuSimulator(
  uint32_t width,
  uint32_t height,
  uint32_t widthWithPadding,
  uint32_t heightWithPadding
) 
  :m_width(width)
  ,m_height(height)
  ,m_widthWithPadding(widthWithPadding)
  ,m_heightWithPadding(heightWithPadding)
{}

auto CpuSimulator::run(
  const std::vector<uint8_t>& bufferIn,
  std::vector<uint8_t>& bufferOut
) -> void {
  assert(bufferIn.size() == m_widthWithPadding * m_heightWithPadding);
  assert(bufferIn.size() == bufferOut.size());

  constexpr uint8_t FALL_RULES[] {
    0b00000000, // PADDING
    0b00000000, // AIR
    0b00000010  // SAND
  };

  for (uint32_t row = 0; row < m_height; ++row) {
    for (uint32_t col = 0; col < m_width; ++col) {
      const uint32_t cellIdx = (row + PADDING_SIZE) * m_widthWithPadding + col + PADDING_SIZE;
      uint32_t otherIdx;
      uint8_t other;

      const uint8_t cell = bufferIn[cellIdx];

      // Fall down
      otherIdx = cellIdx + m_widthWithPadding;
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

      otherIdx = cellIdx - m_widthWithPadding;
      other = bufferIn[otherIdx];
      if ((other & cellRule) > 0) {
        // Can fall down
        const uint32_t otherRuleIdx = std::log2(other);
        const uint8_t otherRule = FALL_RULES[otherRuleIdx];

        const uint32_t otherDownIdx = otherIdx - m_widthWithPadding;
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