#ifndef APPLICATION_SIMULATION_GPU_UTILS_HPP
#define APPLICATION_SIMULATION_GPU_UTILS_HPP

#include "engine/error/Error.hpp"
#include <expected>

auto isGpuBigEndian() -> std::expected<bool, engine::Error>;

#endif