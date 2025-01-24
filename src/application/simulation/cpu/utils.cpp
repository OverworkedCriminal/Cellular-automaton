#include "application/simulation/cpu/utils.hpp"
#include "application/simulation/cpu/colors.hpp"
#include <cmath>

auto mapCellsToColors(
  const std::vector<uint8_t>& inCells,
  std::vector<float>& outColors
) -> void {
  for (uint32_t idx = 0; idx < inCells.size(); ++idx) {
    const uint32_t colorIdx = std::log2(inCells[idx]);
    const Color& color = COLORS[colorIdx];

    for (uint32_t colorComponentIdx = 0; colorComponentIdx < 4; ++colorComponentIdx) {
      outColors[idx * 4 + colorComponentIdx] = color.component[colorComponentIdx];
    }
  }
}