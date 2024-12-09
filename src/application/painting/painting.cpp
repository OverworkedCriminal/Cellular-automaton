#include "application/painting/painting.hpp"

auto paint(
  unsigned int posX,
  unsigned int posY,
  Cell cell,
  std::vector<Cell>& grid,
  unsigned int gridWidth,
  unsigned int gridHeight,
  unsigned int gridPadding,
  unsigned int gridValueOffset,
  unsigned int gridValueStride
) -> void {
  constexpr int SIZE = 2;

  const int lBoundX = gridPadding;
  const int uBoundX = gridWidth - gridPadding;

  const int lBoundY = gridPadding;
  const int uBoundY = gridHeight - gridPadding;

  for (int row = -SIZE; row <= SIZE; ++row) {
    for (int col = -SIZE; col <= SIZE; ++col) {
      int x = static_cast<int>(posX) + col;
      int y = static_cast<int>(posY) + row;
      if (x < lBoundX || x >= uBoundX || y < lBoundY || y >= uBoundY) {
        continue;
      }

      int idx = (y * gridWidth + x) * gridValueStride + gridValueOffset;
      grid[idx] = cell;
    }
  }
}