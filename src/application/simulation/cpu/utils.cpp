#include "application/simulation/cpu/utils.hpp"
#include "application/simulation/cpu/colors.hpp"
#include <cmath>

auto mapCellsToColors(
  const std::vector<Cell>& inCells,
  std::vector<float>& outColors
) -> void {
  for (int idx = 0; idx < inCells.size(); ++idx) {
    const int colorIdx = std::log2(static_cast<uint8_t>(inCells[idx]));
    const auto& color = COLORS[colorIdx];

    for (int colorComponentIdx = 0; colorComponentIdx < 4; ++colorComponentIdx) {
      outColors[idx * 4 + colorComponentIdx] = color.component[colorComponentIdx];
    }
  }
}