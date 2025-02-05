#ifndef APPLICATION_SIMULATION_GPU_UTILS_HPP
#define APPLICATION_SIMULATION_GPU_UTILS_HPP

#include "engine/error/Error.hpp"
#include <expected>
#include <vector>

auto isCpuAndGpuEndianessMatching() -> std::expected<bool, engine::Error>;

auto switchEndianess(std::vector<uint32_t>& buffer) -> void;

#endif