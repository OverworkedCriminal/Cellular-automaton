#ifndef APPLICATION_SIMULATION_CPU_UTILS_HPP
#define APPLICATION_SIMULATION_CPU_UTILS_HPP

#include <cstdint>
#include <vector>

auto mapCellsToColors(
  const std::vector<uint8_t>& buffer,
  std::vector<float>& textureBuffer
) -> void;

#endif