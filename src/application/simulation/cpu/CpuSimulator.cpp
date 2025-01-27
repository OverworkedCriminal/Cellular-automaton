#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/simulation/padding.hpp"
#include "application/simulation/rule.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/error.hpp"
#include "engine/utils/thread_pool/ThreadPool.hpp"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <latch>

using engine::error;
using engine::Size2D;
using engine::Position2D;
using engine::Error;
using engine::ThreadPool;

auto CpuSimulator::create(
  Size2D<uint32_t> size,
  uint32_t processorsCount
) -> std::expected<CpuSimulator, Error> {
  if (size.width < 1 || size.height < 1) {
    return std::unexpected(error("invalid simulation dimensions"));
  }

  if (processorsCount == 0) {
    return std::unexpected(error("processorsCount cannot be 0"));
  }
  const Size2D<float> subrangeSize = {
    .width = static_cast<float>(size.width) / processorsCount,
    .height = static_cast<float>(size.height) / processorsCount
  };

  auto threadPool = ThreadPool::create(processorsCount);
  if (!threadPool.has_value()) {
    return std::unexpected(error("failed to create thread pool", threadPool.error()));
  }

  const Size2D<uint32_t> sizeWithPadding = {
    .width = size.width + 2 * PADDING_SIZE,
    .height = size.height + 2 * PADDING_SIZE
  };

  return CpuSimulator(
    std::move(*threadPool),
    subrangeSize,
    size,
    sizeWithPadding
  );
}

CpuSimulator::CpuSimulator(
  ThreadPool&& threadPool,
  Size2D<float> subrangeSize,
  Size2D<uint32_t> size,
  Size2D<uint32_t> sizeWithPadding
)
  :m_threadPool(std::move(threadPool))
  ,m_subrangeSize(subrangeSize)
  ,m_size(size)
  ,m_sizeWithPadding(sizeWithPadding)
  ,m_priorityDirection(-1)
{}

auto CpuSimulator::run(
  const std::vector<cell_t>& bufferIn,
  std::vector<cell_t>& bufferOut
) -> void {
  assert(bufferIn.size() == m_sizeWithPadding.width * m_sizeWithPadding.height);
  assert(bufferIn.size() == bufferOut.size());

  // Any movement in this direction should take priority
  // over movement in opposite direction
  m_priorityDirection = -m_priorityDirection;

  std::latch latch(m_threadPool.getSize());

  for (uint32_t i = 0; i < m_threadPool.getSize(); ++i) {
    m_threadPool.run([this, &bufferIn, &bufferOut, &latch, i] {
      const int32_t UP = m_sizeWithPadding.width;
      const int32_t DOWN = -m_sizeWithPadding.width;

      const uint32_t rowBeg = m_subrangeSize.height * i;
      const uint32_t rowEnd = m_subrangeSize.height * (i + 1);

      const uint32_t colBeg = 0;
      const uint32_t colEnd = m_size.width;

      for (uint32_t row = rowBeg; row < rowEnd; ++row) {
        for (uint32_t col = colBeg; col < colEnd; ++col) {
          const uint32_t idx = (row + PADDING_SIZE) * m_sizeWithPadding.width + col + PADDING_SIZE;
          const uint32_t ruleIdx = log2(bufferIn[idx]);

          { // MOVE_VERTICALLY
            { // MOVE IN
              const uint32_t otherIdx = idx + UP;
              if (canMoveCell(bufferIn, otherIdx, { 0, -1 }, rule::VERTICAL)) {
                bufferOut[idx] = bufferIn[otherIdx];
                continue;
              }
            }
            { // MOVE OUT
              if (canMoveCell(bufferIn, idx, { 0, -1 }, rule::VERTICAL)) {
                bufferOut[idx] = bufferIn[idx + DOWN];
                continue;
              }
            }
          }

          { // MOVE DIAGONALY
            { // MOVE IN
              const uint32_t otherIdx = idx + UP - m_priorityDirection;
              if (
                canMoveCell(bufferIn, otherIdx, { m_priorityDirection, -1 }, rule::DIAGONAL) &&
                !canMoveCell(bufferIn, otherIdx, { 0, -1 }, rule::VERTICAL) &&
                !canMoveCell(bufferIn, otherIdx + UP, { 0, -1 }, rule::VERTICAL)
              ) {
                bufferOut[idx] = bufferIn[otherIdx];
                continue;
              }
            }
            { // MOVE OUT
              const uint32_t otherIdx = idx + DOWN + m_priorityDirection;
              if (
                canMoveCell(bufferIn, idx, { m_priorityDirection, -1 }, rule::DIAGONAL) &&
                !canMoveCell(bufferIn, otherIdx, { 0, -1 }, rule::VERTICAL) &&
                !canMoveCell(bufferIn, otherIdx + UP, { 0, -1 }, rule::VERTICAL)
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
              const int32_t otherDirection = rule::HORIZONTAL_DIRECTIONS[otherRuleIdx];

              if (
                otherDirection == m_priorityDirection &&
                canMoveCell(bufferIn, otherIdx, { m_priorityDirection, 0 }, rule::HORIZONTAL) &&
                !canMoveCell(bufferIn, otherIdx, { 0, -1 }, rule::VERTICAL) &&
                !canMoveCell(bufferIn, otherIdx + UP, { 0, -1 }, rule::VERTICAL) &&
                !canMoveCell(bufferIn, otherIdx, { m_priorityDirection, -1 }, rule::DIAGONAL) &&
                !canMoveCell(bufferIn, otherIdx + UP - m_priorityDirection, { m_priorityDirection, -1 }, rule::DIAGONAL)
              ) {
                bufferOut[idx] = bufferIn[otherIdx];
                continue;
              }
            }
            { // MOVE OUT
              const int32_t direction = rule::HORIZONTAL_DIRECTIONS[ruleIdx];
              const uint32_t otherIdx = idx + m_priorityDirection;
              if (direction == m_priorityDirection) {
                if (
                  canMoveCell(bufferIn, idx, { m_priorityDirection, 0 }, rule::HORIZONTAL) &&
                  !canMoveCell(bufferIn, otherIdx, { 0, -1 }, rule::VERTICAL) &&
                  !canMoveCell(bufferIn, otherIdx + UP, { 0, -1 }, rule::VERTICAL) &&
                  !canMoveCell(bufferIn, otherIdx, { m_priorityDirection, -1 }, rule::DIAGONAL) &&
                  !canMoveCell(bufferIn, otherIdx + UP - m_priorityDirection, { m_priorityDirection, -1 }, rule::DIAGONAL)
                ) {
                  bufferOut[idx] = bufferIn[otherIdx];
                } else {
                  bufferOut[idx] = rule::HORIZONTAL_OPPOSITE_DIRECTION_CELL[ruleIdx];
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

      latch.count_down();
    });
  }

  latch.wait();  
}

auto CpuSimulator::canMoveCell(
  const std::vector<cell_t>& bufferIn,
  uint32_t cellIdx,
  Position2D<int32_t> direction,
  const std::array<cell_t, 5>& rules
) const -> bool {
  const cell_t cell = bufferIn[cellIdx];
  const uint8_t cellRuleIdx = log2(cell);
  const cell_t cellRule = rules[cellRuleIdx];

  const uint32_t otherIdx = cellIdx + direction.y * m_sizeWithPadding.width + direction.x;
  const cell_t other = bufferIn[otherIdx];

  return (other & cellRule) > 0;
}
