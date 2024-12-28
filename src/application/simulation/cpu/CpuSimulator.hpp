#ifndef APPLICATION_SIMULATION_CPU_CPU_SIMULATOR_HPP
#define APPLICATION_SIMULATION_CPU_CPU_SIMULATOR_HPP

#include "engine/error/Error.hpp"
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
   * @param width (does not include padding)
   * @param height (does not include padding)
   *
   * @return std::expected<CpuSimulator, engine::Error>
   *
   * @throws engine::Error when width or height is less than 1
   */
  static auto create(
    uint32_t width,
    uint32_t height
  ) -> std::expected<CpuSimulator, engine::Error>;

  CpuSimulator(const CpuSimulator&) = delete;
  CpuSimulator(CpuSimulator&&) = default;

  auto operator=(const CpuSimulator&) -> CpuSimulator& = delete;
  auto operator=(CpuSimulator&&) -> CpuSimulator& = default;

  auto run(
    const std::vector<uint8_t>& bufferIn,
    std::vector<uint8_t>& bufferOut
  ) -> void;

private:
  CpuSimulator(
    uint32_t width,
    uint32_t height,
    uint32_t widthWithPadding,
    uint32_t heightWithPadding
  );

  uint32_t m_width;
  uint32_t m_height;
  uint32_t m_widthWithPadding;
  uint32_t m_heightWithPadding;

};

#endif