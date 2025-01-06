#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/simulation/padding.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/error.hpp"
#include <cassert>
#include <cmath>
#include <cstdint>

using std::array;
using std::vector;
using engine::error;
using engine::Size2D;
using engine::Position2D;

static constexpr array<uint8_t, 4> FALL_DOWN_RULES = {
  0b00000000, // PADDING
  0b00000000, // AIR
  0b00001010, // SAND
  0b00000010  // WATER
};

static constexpr array<uint8_t, 4> FALL_DIAG_RULES = {
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
      uint32_t otherIdx;

      otherIdx = cellIdx - m_sizeWithPadding.width;
      if (const auto otherCell = canMoveCell(bufferIn, cellIdx, { 0, -1 }, FALL_DOWN_RULES)) {
        // Can fall down
        if (!canMoveCell(bufferIn, otherIdx, { 0, -1 }, FALL_DOWN_RULES)) {
          // Cell below can't fall down
          bufferOut[cellIdx] = bufferIn[otherIdx];
          continue;
        }
      }

      // Can't fall down

      otherIdx = cellIdx + m_sizeWithPadding.width;
      if (canMoveCell(bufferIn, otherIdx, { 0, -1 }, FALL_DOWN_RULES)) {
        // Cell above can fall here
        bufferOut[cellIdx] = bufferIn[otherIdx];
        continue;
      }

      // Can't fall down here

      otherIdx = cellIdx - 1 - m_sizeWithPadding.width;
      if (canMoveCell(bufferIn, cellIdx, { -1, -1 }, FALL_DIAG_RULES)) {
        // Can fall diagonally left
        if (
          !canMoveCell(bufferIn, otherIdx + m_sizeWithPadding.width, { 0, -1 }, FALL_DOWN_RULES) &&
          !canMoveCell(bufferIn, otherIdx, { 0, -1 }, FALL_DOWN_RULES)
        ) {
          bufferOut[cellIdx] = bufferIn[otherIdx];
          continue;
        }
      }

      // Can't fall diagonally left

      otherIdx = cellIdx + 1 - m_sizeWithPadding.width;
      if (canMoveCell(bufferIn, cellIdx, { 1, -1 }, FALL_DIAG_RULES)) {
        // Can fall diagonally right
        if (
          !canMoveCell(bufferIn, otherIdx + m_sizeWithPadding.width, { 0, -1 }, FALL_DOWN_RULES) &&
          !canMoveCell(bufferIn, otherIdx, { 0, -1 }, FALL_DOWN_RULES)
        ) {
          bufferOut[cellIdx] = bufferIn[otherIdx];
          continue;
        }
      }

      // Can't fall diagonally right

      otherIdx = cellIdx + m_sizeWithPadding.width + 1;
      if (canMoveCell(bufferIn, otherIdx, { -1, -1 }, FALL_DIAG_RULES)) {
        // Can fall diagonally left here
        if (
          !canMoveCell(bufferIn, otherIdx, { 0, -1 }, FALL_DOWN_RULES) &&
          !canMoveCell(bufferIn, otherIdx + m_sizeWithPadding.width, { 0, -1 }, FALL_DOWN_RULES)
        ) {
          bufferOut[cellIdx] = bufferIn[otherIdx];
          continue;
        }
      }

      // Other can't fall diagonally left here

      otherIdx = cellIdx + m_sizeWithPadding.width - 1;
      if (canMoveCell(bufferIn, otherIdx, { 1, -1 }, FALL_DIAG_RULES)) {
        // Can fall diagonally right here
        if (
          !canMoveCell(bufferIn, otherIdx, { 0, -1 }, FALL_DOWN_RULES) &&
          !canMoveCell(bufferIn, otherIdx + m_sizeWithPadding.width, { 0, -1 }, FALL_DOWN_RULES) &&
          !canMoveCell(bufferIn, otherIdx, { -1, -1 }, FALL_DIAG_RULES)
        ) {
          bufferOut[cellIdx] = bufferIn[otherIdx];
          continue;
        }
      }

      // Other can't fall diagonally right here

      // If no rule applies to cell at cellIdx
      // just copy it to the bufferOut
      bufferOut[cellIdx] = bufferIn[cellIdx];
    }
  }
}

auto CpuSimulator::canMoveCell(
  const vector<uint8_t>& bufferIn,
  uint32_t cellIdx,
  Position2D<int32_t> direction,
  const array<uint8_t, 4>& rules
) const -> bool {
  const uint8_t cell = bufferIn[cellIdx];
  const uint8_t cellRuleIdx = std::log2(cell);
  const uint8_t cellRule = rules[cellRuleIdx];

  const uint32_t otherIdx = cellIdx + direction.y * m_sizeWithPadding.width + direction.x;
  const uint8_t other = bufferIn[otherIdx];

  return (other & cellRule) > 0;
}
