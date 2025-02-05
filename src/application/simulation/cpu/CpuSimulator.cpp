#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/simulation/padding.hpp"
#include "application/simulation/rule.hpp"
#include "engine/utils/error.hpp"
#include "engine/utils/thread_pool/ThreadPool.hpp"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <latch>

using engine::error;
using engine::Size2D;
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
  const std::vector<cell_t>& inputBuffer,
  std::vector<cell_t>& outputBuffer
) -> void {
  assert(inputBuffer.size() == m_sizeWithPadding.width * m_sizeWithPadding.height);
  assert(inputBuffer.size() == outputBuffer.size());

  // Any movement in this direction should take priority
  // over movement in opposite direction
  m_priorityDirection = -m_priorityDirection;

  std::latch latch(m_threadPool.getSize());

  for (uint32_t i = 0; i < m_threadPool.getSize(); ++i) {
    m_threadPool.run([this, &inputBuffer, &outputBuffer, &latch, i] {
      const uint32_t rowBeg = m_subrangeSize.height * i;
      const uint32_t rowEnd = m_subrangeSize.height * (i + 1);

      const uint32_t colBeg = 0;
      const uint32_t colEnd = m_size.width;

      for (uint32_t row = rowBeg; row < rowEnd; ++row) {
        for (uint32_t col = colBeg; col < colEnd; ++col) {
          const uint32_t idx = (row + PADDING_SIZE) * m_sizeWithPadding.width + col + PADDING_SIZE;
          const uint32_t ruleIdx = std::log2(inputBuffer[idx]);

          { // MOVE_VERTICALLY
            { // MOVE OUT
              const uint32_t otherIdx = idx - m_sizeWithPadding.width;
              const uint32_t otherRuleIdx = std::log2(inputBuffer[otherIdx]);
              if ((inputBuffer[otherIdx] & rule::VERTICAL[ruleIdx]) > 0) {
                const uint32_t otherDownIdx = otherIdx - m_sizeWithPadding.width;
                if ((inputBuffer[otherDownIdx] & rule::VERTICAL[otherRuleIdx]) == 0) { // other can't move down
                  outputBuffer[idx] = inputBuffer[otherIdx];
                  continue;
                }
              }
            }
            { // MOVE IN
              const uint32_t otherIdx = idx + m_sizeWithPadding.width;
              const uint32_t otherRuleIdx = std::log2(inputBuffer[otherIdx]);
              if ((inputBuffer[idx] & rule::VERTICAL[otherRuleIdx]) > 0) { // other can move down
                outputBuffer[idx] = inputBuffer[otherIdx];
                continue;
              }
            }
          }

          { // MOVE_DIAGONALLY
            { // MOVE OUT
              const uint32_t otherIdx = idx - m_sizeWithPadding.width + m_priorityDirection;
              if ((inputBuffer[otherIdx] & rule::DIAGONAL[ruleIdx]) > 0) { // can move diagonally
                const uint32_t otherRuleIdx = std::log2(inputBuffer[otherIdx]);
                const uint32_t otherTopIdx = otherIdx + m_sizeWithPadding.width;
                const uint32_t otherTopRuleIdx = std::log2(inputBuffer[otherTopIdx]);
                const uint32_t otherDownIdx = otherIdx - m_sizeWithPadding.width;
                const uint32_t otherDiagonalIdx = otherIdx - m_sizeWithPadding.width + m_priorityDirection;
                if (
                  (inputBuffer[otherIdx] & rule::VERTICAL[otherTopRuleIdx]) == 0 &&   // other top can't move vertically
                  (inputBuffer[otherDownIdx] & rule::VERTICAL[otherRuleIdx]) == 0 &&  // other can't move vertically
                  (inputBuffer[otherDiagonalIdx] & rule::DIAGONAL[otherRuleIdx]) == 0 // other can't move diagonally
                ) {
                  outputBuffer[idx] = inputBuffer[otherIdx];
                  continue;
                }
              }
            }
            { // MOVE IN
              const uint32_t otherIdx = idx + m_sizeWithPadding.width - m_priorityDirection;
              const uint32_t otherRuleIdx = std::log2(inputBuffer[otherIdx]);
              if ((inputBuffer[idx] & rule::DIAGONAL[otherRuleIdx]) > 0) { // other can move diagonally
                const uint32_t otherTopIdx = otherIdx + m_sizeWithPadding.width;
                const uint32_t otherTopRuleIdx = std::log2(inputBuffer[otherTopIdx]);
                const uint32_t otherDownIdx = otherIdx - m_sizeWithPadding.width;
                if (
                  (inputBuffer[otherIdx] & rule::VERTICAL[otherTopRuleIdx]) == 0 && // other top can't move vertically
                  (inputBuffer[otherDownIdx] & rule::VERTICAL[otherRuleIdx]) == 0   // other can't move vertically
                ) {
                  outputBuffer[idx] = inputBuffer[otherIdx];
                  continue;
                }
              }
            }
          }

          { // MOVE_HORIZONTALLY
            bool shouldChangeDirection = false;
            uint directionToChangeTo;

            { // MOVE OUT
              const int32_t direction = rule::HORIZONTAL_DIRECTIONS[ruleIdx];
              const uint32_t otherIdx = idx + m_priorityDirection;
              if (direction == m_priorityDirection) { // simulation processes right direction
                const uint32_t otherRuleIdx = std::log2(inputBuffer[otherIdx]);
                const uint32_t otherDirectionIdx = otherIdx + m_priorityDirection;
                const uint32_t otherDownIdx = otherIdx - m_sizeWithPadding.width;
                const uint32_t otherTopIdx = otherIdx + m_sizeWithPadding.width;
                const uint32_t otherTopRuleIdx = std::log2(inputBuffer[otherTopIdx]);
                const uint32_t otherDownDiagIdx = otherIdx - m_sizeWithPadding.width + m_priorityDirection;
                const uint32_t otherTopDiagIdx = otherIdx + m_sizeWithPadding.width - m_priorityDirection;
                const uint32_t otherTopDiagRuleIdx = std::log2(inputBuffer[otherTopDiagIdx]);
                if (
                  (inputBuffer[otherIdx] & rule::HORIZONTAL[ruleIdx]) > 0 &&                // can move horizontally
                  (inputBuffer[otherDirectionIdx] & rule::HORIZONTAL[otherRuleIdx]) == 0 && // other can't move horizontally
                  (inputBuffer[otherDownIdx] & rule::VERTICAL[otherRuleIdx]) == 0 &&        // other can't move vertically
                  (inputBuffer[otherIdx] & rule::VERTICAL[otherTopRuleIdx]) == 0 &&         // other top can't move vertically
                  (inputBuffer[otherDownDiagIdx] & rule::DIAGONAL[otherRuleIdx]) == 0 &&    // other can't move diagonally
                  (inputBuffer[otherIdx] & rule::DIAGONAL[otherTopDiagRuleIdx]) == 0        // other top diag can't move diagonally
                ) {
                  outputBuffer[idx] = inputBuffer[otherIdx];
                  continue;
                }

                shouldChangeDirection = true;
                directionToChangeTo = rule::HORIZONTAL_OPPOSITE_DIRECTION_CELL[ruleIdx];
              }
            }
            { // MOVE IN
              const uint32_t otherIdx = idx - m_priorityDirection;
              const uint32_t otherRuleIdx = std::log2(inputBuffer[otherIdx]);
              const int32_t otherDirection = rule::HORIZONTAL_DIRECTIONS[otherRuleIdx];
              if (otherDirection == m_priorityDirection) { // simulation processes right direction
                const uint32_t otherDownIdx = otherIdx - m_sizeWithPadding.width;
                const uint32_t otherTopIdx = otherIdx + m_sizeWithPadding.width;
                const uint32_t otherTopRuleIdx = std::log2(inputBuffer[otherTopIdx]);
                const uint32_t otherDownDiagIdx = otherIdx - m_sizeWithPadding.width + m_priorityDirection;
                const uint32_t otherTopDiagIdx = otherIdx + m_sizeWithPadding.width - m_priorityDirection;
                const uint32_t otherTopDiagRuleIdx = std::log2(inputBuffer[otherTopDiagIdx]);
                if (
                  (inputBuffer[idx] & rule::HORIZONTAL[otherRuleIdx]) > 0 &&             // other can move horizontally
                  (inputBuffer[otherDownIdx] & rule::VERTICAL[otherRuleIdx]) == 0 &&     // other can't move vertically
                  (inputBuffer[otherIdx] & rule::VERTICAL[otherTopRuleIdx]) == 0 &&      // other top can't move vertically
                  (inputBuffer[otherDownDiagIdx] & rule::DIAGONAL[otherRuleIdx]) == 0 && // other can't move diagonally
                  (inputBuffer[otherIdx] & rule::DIAGONAL[otherTopDiagRuleIdx]) == 0     // other top diag can't move diagonally
                ) {
                  outputBuffer[idx] = inputBuffer[otherIdx];
                  continue;
                }
              }
            }
            { // WANTS TO MOVE BUT CAN'T
              if (shouldChangeDirection) {
                outputBuffer[idx] = directionToChangeTo;
                continue;
              }
            }
          }

          // If no rule applies to cell at cellIdx
          // just copy it to the bufferOut
          outputBuffer[idx] = inputBuffer[idx];
        }
      }

      latch.count_down();
    });
  }

  latch.wait();  
}
