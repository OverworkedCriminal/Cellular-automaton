#ifndef APPLICATION_SIMULATION_CPU_CPU_SIMULATOR_HPP
#define APPLICATION_SIMULATION_CPU_CPU_SIMULATOR_HPP

#include "engine/error/Error.hpp"
#include "engine/utils/dto/Size2D.hpp"
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
   *
   * @return std::expected<CpuSimulator, engine::Error>
   *
   * @throws engine::Error when width or height is less than 1
   */
  static auto create(engine::Size2D<uint32_t> size) -> std::expected<CpuSimulator, engine::Error>;

  CpuSimulator(const CpuSimulator&) = delete;
  CpuSimulator(CpuSimulator&&) = default;

  auto operator=(const CpuSimulator&) -> CpuSimulator& = delete;
  auto operator=(CpuSimulator&&) -> CpuSimulator& = default;

  auto run(
    const std::vector<uint8_t>& bufferIn,
    std::vector<uint8_t>& bufferOut
  ) const -> void;

private:
  CpuSimulator(
    engine::Size2D<uint32_t> size,
    engine::Size2D<uint32_t> sizeWithPadding
  );

  engine::Size2D<uint32_t> m_size;
  engine::Size2D<uint32_t> m_sizeWithPadding;

  auto fallStraight(const std::vector<uint8_t>& bufferIn, uint32_t cellIdx) const -> std::optional<uint8_t>;
};

#endif