#ifndef APPLICATION_PAINTING_PAINTING_HPP
#define APPLICATION_PAINTING_PAINTING_HPP

#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <cstdint>

/**
 * @brief Maps position from window space to simulation space
 * 
 * @param windowPosition 
 * @param framebufferSize 
 * @param simulationSize (including padding added to each side of the simulation canvas)
 * 
 * @return engine::Position2D<uint32_t> 
 */
auto mapWindowPositionToSimulationPosition(
  engine::Position2D<uint32_t> windowPosition,
  engine::Size2D<uint32_t> framebufferSize,
  engine::Size2D<uint32_t> simulationSize
) -> engine::Position2D<uint32_t>;

#endif