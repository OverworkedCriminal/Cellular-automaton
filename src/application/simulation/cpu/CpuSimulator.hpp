#ifndef APPLICATION_SIMULATION_CPU_CPU_SIMULATOR_HPP
#define APPLICATION_SIMULATION_CPU_CPU_SIMULATOR_HPP

#include "application/simulation/Cell.hpp"
#include "engine/error/Error.hpp"
#include <expected>
#include <vector>

class CpuSimulator {
public:
  static auto create(
    int width,
    int height
  ) -> std::expected<CpuSimulator, engine::Error>;

  CpuSimulator(const CpuSimulator&) = delete;
  CpuSimulator(CpuSimulator&&) = default;

  auto operator=(const CpuSimulator&) -> CpuSimulator& = delete;
  auto operator=(CpuSimulator&&) -> CpuSimulator& = default;

  auto run(
    const std::vector<Cell>& bufferIn,
    std::vector<Cell>& bufferOut
  ) -> void;

private:
  CpuSimulator(
    int width,
    int height,
    int widthWithPadding,
    int heightWithPadding
  );

  int m_width;
  int m_height;
  int m_widthWithPadding;
  int m_heightWithPadding;

};

#endif