#include "application/simulation/SimulationGrid.hpp"
#include "engine/utils/error.hpp"

using engine::error;

auto SimulationGrid::create(
  std::vector<std::byte>* buffer,
  unsigned int width,
  unsigned int height,
  unsigned int cellSize,
  unsigned int cellValueOffset
) -> std::expected<SimulationGrid, engine::Error> {
  if (buffer->size() < width * height * cellSize) {
    return std::unexpected(error("buffer is too small"));
  }

  if (cellValueOffset >= cellSize) {
    return std::unexpected(error("cellValueOffset must be lower than cellSize"));
  }

  return SimulationGrid(
    buffer,
    width,
    height,
    cellSize,
    cellValueOffset
  );
}

SimulationGrid::SimulationGrid(
  std::vector<std::byte>* buffer,
  unsigned int width,
  unsigned int height,
  unsigned int cellSize,
  unsigned int cellValueOffset
)
  :m_buffer(buffer)
  ,m_width(width)
  ,m_height(height)
  ,m_cellSize(cellSize)
  ,m_cellValueOffset(cellValueOffset)
{}

auto SimulationGrid::get(unsigned int x, unsigned int y) -> std::byte {
  const auto& buffer = *m_buffer;

  const auto idx = bufferIndex(x, y);

  return buffer[idx];
}

auto SimulationGrid::set(unsigned int x, unsigned int y, std::byte value) -> void {
  auto& buffer = *m_buffer;

  const auto idx = bufferIndex(x, y);

  buffer[idx] = value;
}

auto SimulationGrid::bufferIndex(unsigned int x, unsigned int y) -> unsigned int {
  return (y * m_width + x) * m_cellSize + m_cellValueOffset;
}