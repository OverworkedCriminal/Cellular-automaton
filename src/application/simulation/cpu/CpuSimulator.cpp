#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/simulation/cell.hpp"
#include "application/simulation/padding.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/error.hpp"
#include <cassert>
#include <cmath>
#include <cstdint>

using std::array;
using std::vector;
using std::log2;
using engine::error;
using engine::Size2D;
using engine::Position2D;

static constexpr array<uint8_t, 5> MOVE_DOWN_RULES = {
  0b00000000, // PADDING
  0b00000000, // AIR
  0b00011010, // SAND
  0b00000010, // WATER_L
  0b00000010  // WATER_R
};

static constexpr array<uint8_t, 5> MOVE_DOWN_DIAG_RULES = {
  0b00000000, // PADDING
  0b00000000, // AIR
  0b00000010, // SAND
  0b00000010, // WATER_L
  0b00000010  // WATER_R
};

static constexpr array<uint8_t, 5> MOVE_HORIZONTALLY_RULES = {
  0b00000000, // PADDING
  0b00000000, // AIR
  0b00000000, // SAND
  0b00010010, // WATER_L
  0b00001010  // WATER_R
};

static constexpr array<int32_t, 5> MOVE_HORIZONTALLY_DIRECTIONS = {
   0, // PADDING
   0, // AIR
   0, // SAND
  -1, // WATER_L
   1  // WATER_R
};

static constexpr array<int32_t, 5> MOVE_HORIZONTALLY_OPPOSITE_CELL = {
  cell::PADDING, // PADDING
  cell::AIR,     // AIR
  cell::SAND,    // SAND
  cell::WATER_R, // WATER_L
  cell::WATER_L  // WATER_R
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
  ,m_priorityDirection(-1)
{}

auto CpuSimulator::run(
  const vector<uint8_t>& bufferIn,
  vector<uint8_t>& bufferOut
) -> void {
  assert(bufferIn.size() == m_sizeWithPadding.width * m_sizeWithPadding.height);
  assert(bufferIn.size() == bufferOut.size());

  // Any movement in this direction should take priority
  // over movement in opposite direction
  m_priorityDirection = -m_priorityDirection;

  const int32_t UP = m_sizeWithPadding.width;
  const int32_t DOWN = -m_sizeWithPadding.width;
  const int32_t LEFT = -1;
  const int32_t RIGHT = 1;

  for (uint32_t row = 0; row < m_size.height; ++row) {
    for (uint32_t col = 0; col < m_size.width; ++col) {
      const uint32_t idx = (row + PADDING_SIZE) * m_sizeWithPadding.width + col + PADDING_SIZE;
      const uint32_t ruleIdx = log2(bufferIn[idx]);

      { // MOVE_VERTICALY
        { // MOVE IN
          const uint32_t otherIdx = idx + UP;
          if (canMoveCell(bufferIn, otherIdx, { 0, -1 }, MOVE_DOWN_RULES)) {
            bufferOut[idx] = bufferIn[otherIdx];
            continue;
          }
        }
        { // MOVE OUT
          if (canMoveCell(bufferIn, idx, { 0, -1 }, MOVE_DOWN_RULES)) {
            bufferOut[idx] = bufferIn[idx + DOWN];
            continue;
          }
        }
      }

      { // MOVE DIAGONALY
        { // MOVE IN
          const uint32_t otherIdx = idx + UP - m_priorityDirection;
          if (
            canMoveCell(bufferIn, otherIdx, { m_priorityDirection, -1 }, MOVE_DOWN_DIAG_RULES) &&
            !canMoveCell(bufferIn, otherIdx, { 0, -1 }, MOVE_DOWN_RULES) &&
            !canMoveCell(bufferIn, otherIdx + UP, { 0, -1 }, MOVE_DOWN_RULES)
          ) {
            bufferOut[idx] = bufferIn[otherIdx];
            continue;
          }
        }
        { // MOVE OUT
          const uint32_t otherIdx = idx + DOWN + m_priorityDirection;
          if (
            canMoveCell(bufferIn, idx, { m_priorityDirection, -1 }, MOVE_DOWN_DIAG_RULES) &&
            !canMoveCell(bufferIn, otherIdx, { 0, -1 }, MOVE_DOWN_RULES) &&
            !canMoveCell(bufferIn, otherIdx + UP, { 0, -1 }, MOVE_DOWN_RULES)
          ) {
            bufferOut[idx] = bufferIn[otherIdx];
            continue;
          }
        }
      }

      { // MOVE_HORIZONTALLY
        { // MOVE IN
          const uint32_t otherIdx = idx - m_priorityDirection;
          const uint32_t otherRuleIdx = log2(bufferIn[otherIdx]);
          const int32_t otherDirection = MOVE_HORIZONTALLY_DIRECTIONS[otherRuleIdx];

          if (
            otherDirection == m_priorityDirection &&
            canMoveCell(bufferIn, otherIdx, { m_priorityDirection, 0 }, MOVE_HORIZONTALLY_RULES) &&
            !canMoveCell(bufferIn, otherIdx, { 0, -1 }, MOVE_DOWN_RULES) &&
            !canMoveCell(bufferIn, otherIdx + UP, { 0, -1 }, MOVE_DOWN_RULES) &&
            !canMoveCell(bufferIn, otherIdx, { m_priorityDirection, -1 }, MOVE_DOWN_DIAG_RULES) &&
            !canMoveCell(bufferIn, otherIdx + UP - m_priorityDirection, { m_priorityDirection, -1 }, MOVE_DOWN_DIAG_RULES)
          ) {
            bufferOut[idx] = bufferIn[otherIdx];
            continue;
          }
        }
        { // MOVE OUT
          const int32_t direction = MOVE_HORIZONTALLY_DIRECTIONS[ruleIdx];
          const uint32_t otherIdx = idx + m_priorityDirection;
          if (direction == m_priorityDirection) {
            if (
              canMoveCell(bufferIn, idx, { m_priorityDirection, 0 }, MOVE_HORIZONTALLY_RULES) &&
              !canMoveCell(bufferIn, otherIdx, { 0, -1 }, MOVE_DOWN_RULES) &&
              !canMoveCell(bufferIn, otherIdx + UP, { 0, -1 }, MOVE_DOWN_RULES) &&
              !canMoveCell(bufferIn, otherIdx, { m_priorityDirection, -1 }, MOVE_DOWN_DIAG_RULES) &&
              !canMoveCell(bufferIn, otherIdx + UP - m_priorityDirection, { m_priorityDirection, -1 }, MOVE_DOWN_DIAG_RULES)
            ) {
              bufferOut[idx] = bufferIn[otherIdx];
            } else {
              bufferOut[idx] = MOVE_HORIZONTALLY_OPPOSITE_CELL[ruleIdx];
            }
            continue;
          }
        }
      }

      // If no rule applies to cell at cellIdx
      // just copy it to the bufferOut
      bufferOut[idx] = bufferIn[idx];
    }
  }
}

auto CpuSimulator::canMoveCell(
  const vector<uint8_t>& bufferIn,
  uint32_t cellIdx,
  Position2D<int32_t> direction,
  const array<uint8_t, 5>& rules
) const -> bool {
  const uint8_t cell = bufferIn[cellIdx];
  const uint8_t cellRuleIdx = log2(cell);
  const uint8_t cellRule = rules[cellRuleIdx];

  const uint32_t otherIdx = cellIdx + direction.y * m_sizeWithPadding.width + direction.x;
  const uint8_t other = bufferIn[otherIdx];

  return (other & cellRule) > 0;
}
