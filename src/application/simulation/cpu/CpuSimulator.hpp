#ifndef APPLICATION_SIMULATION_CPU_CPU_SIMULATOR_HPP
#define APPLICATION_SIMULATION_CPU_CPU_SIMULATOR_HPP

#include "application/simulation/cell.hpp"
#include "engine/error/Error.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include "engine/utils/thread_pool/ThreadPool.hpp"
#include <cstdint>
#include <expected>
#include <vector>

/**
 * @brief Class runs simulation on the CPU
 */
class CpuSimulator {
public:

  /**
   * @brief Class constructor
   * 
   * @param size (does not include padding)
   * @param processorsCount (enables parallelism)
   *
   * @return std::expected<CpuSimulator, engine::Error>
   *
   * @throws engine::Error when width or height is less than 1 or processorsCount is 0
   */
  static auto create(
    engine::Size2D<uint32_t> size,
    uint32_t processorsCount = 1
  ) -> std::expected<CpuSimulator, engine::Error>;

  CpuSimulator(const CpuSimulator&) = delete;
  CpuSimulator(CpuSimulator&&) = default;

  auto operator=(const CpuSimulator&) -> CpuSimulator& = delete;
  auto operator=(CpuSimulator&&) -> CpuSimulator& = default;

  auto run(
    const std::vector<cell_t>& bufferIn,
    std::vector<cell_t>& bufferOut
  ) -> void;

private:
  CpuSimulator(
    engine::ThreadPool&& threadPool,
    engine::Size2D<float> subrangeSize,
    engine::Size2D<uint32_t> size,
    engine::Size2D<uint32_t> sizeWithPadding
  );

  engine::ThreadPool m_threadPool;
  engine::Size2D<float> m_subrangeSize;

  engine::Size2D<uint32_t> m_size;
  engine::Size2D<uint32_t> m_sizeWithPadding;

  int32_t m_priorityDirection;

  auto canMoveCell(
    const std::vector<cell_t>& bufferIn,
    uint32_t cellIdx,
    engine::Position2D<int32_t> direction,
    const std::array<cell_t, 5>& rules
  ) const -> bool;
};

#endif