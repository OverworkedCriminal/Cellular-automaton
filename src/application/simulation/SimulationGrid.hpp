#ifndef APPLICATION_SIMULATION_SIMULATION_GRID_HPP
#define APPLICATION_SIMULATION_SIMULATION_GRID_HPP

#include "engine/error/Error.hpp"
#include <cstddef>
#include <expected>
#include <vector>

/**
 * @brief
 * Wraps byte buffer to make data access easier.
 * SimulationGrid does not take ownership of the buffer,
 * it means buffer needs to live longer than SimulationGrid.
 */
class SimulationGrid {
public:
  /**
   * @brief 
   * Create SimulationGrid.
   *
   * Grid stores <0, 255> values but those values can be encoded on multiple bytes.
   * cellValueOffset <0, cellSize) allows to specify where this value is located
   * (within a cell), 0 for lowest address.
   *
   * When cellSize is 1 cellValueOffset should be 0
   * 
   * @param buffer
   * @param width width of the grid (in elements)
   * @param height height of the grid (in elements)
   * @param cellSize size of the single cell (in bytes)
   * @param cellValueOffset offset of cell value within a cell (in bytes)
   * @return std::expected<SimulationGrid, engine::Error> 
   */
  static auto create(
    std::vector<std::byte>* buffer,
    unsigned int width,
    unsigned int height,
    unsigned int cellSize = 1,
    unsigned int cellValueOffset = 0
  ) -> std::expected<SimulationGrid, engine::Error>;

  SimulationGrid(const SimulationGrid&) = default;
  SimulationGrid(SimulationGrid&&) = default;

  auto operator=(const SimulationGrid&) -> SimulationGrid& = default;
  auto operator=(SimulationGrid&&) -> SimulationGrid& = default;

  auto getCell(unsigned int x, unsigned int y) -> std::byte;
  auto setCell(unsigned int x, unsigned int y, std::byte value) -> void;

  auto getWidth() -> unsigned int;
  auto getHeight() -> unsigned int;

  auto swapBuffer(std::vector<std::byte>* buffer) -> std::expected<void, engine::Error>;

private:
  SimulationGrid(
    std::vector<std::byte>* buffer,
    unsigned int width,
    unsigned int height,
    unsigned int cellSize,
    unsigned int valueOffset
  );

  auto bufferIndex(unsigned int x, unsigned int y) -> unsigned int;

  std::vector<std::byte>* m_buffer;

  unsigned int m_width;
  unsigned int m_height;

  unsigned int m_cellSize;
  unsigned int m_cellValueOffset;
};

#endif