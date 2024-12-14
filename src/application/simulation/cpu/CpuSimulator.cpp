#include "application/simulation/cpu/CpuSimulator.hpp"
#include "engine/utils/error.hpp"
#include <cmath>

using engine::error;

constexpr int PADDING = 2;

auto CpuSimulator::create(
  int width,
  int height
) -> std::expected<CpuSimulator, engine::Error> {
  if (width <= 2 * PADDING || height <= 2 * PADDING) {
    return std::unexpected(error("invalid simulation dimensions"));
  }

  return CpuSimulator(
    width - 2 * PADDING,
    height - 2 * PADDING,
    width,
    height
  );
}

CpuSimulator::CpuSimulator(
  int width,
  int height,
  int widthWithPadding,
  int heightWithPadding
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
  constexpr int FALL_RULES[] {
    0b00000000, // PADDING
    0b00000000, // AIR
    0b00000010  // SAND
  };

  for (int row = 0; row < m_height; ++row) {
    for (int col = 0; col < m_width; ++col) {
      const int cellIdx = (row + PADDING) * m_widthWithPadding + col + PADDING;
      int otherIdx;
      uint8_t other;

      const uint8_t cell = static_cast<uint8_t>(bufferIn[cellIdx]);

      // Fall down
      otherIdx = cellIdx + m_widthWithPadding;
      other = static_cast<uint8_t>(bufferIn[otherIdx]);
      const int otherRuleIdx = std::log2(other);
      const int otherRule = FALL_RULES[otherRuleIdx];
      if ((cell & otherRule) > 0) {
        // Other can fall here
        bufferOut[cellIdx] = bufferIn[otherIdx];
        continue;
      }

      const int cellRuleIdx = std::log2(cell);
      const int cellRule = FALL_RULES[cellRuleIdx];

      otherIdx = cellIdx - m_widthWithPadding;
      other = static_cast<uint8_t>(bufferIn[otherIdx]);
      if ((other & cellRule) > 0) {
        // Can fall down
        const int otherRuleIdx = std::log2(other);
        const int otherRule = FALL_RULES[otherRuleIdx];

        const int otherDownIdx = otherIdx - m_widthWithPadding;
        const int otherDown = static_cast<uint8_t>(bufferIn[otherDownIdx]);
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