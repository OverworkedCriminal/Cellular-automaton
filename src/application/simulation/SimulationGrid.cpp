#include "application/simulation/SimulationGrid.hpp"
#include "engine/utils/error.hpp"

using engine::error;

static auto validateBuffer(
  std::vector<std::byte>* buffer,
  unsigned int width,
  unsigned int height,
  unsigned int cellSize
) -> std::expected<void, engine::Error> {
  if (buffer == nullptr) {
    return std::unexpected(error("buffer can't' be nullptr"));
  }

  if (buffer->size() < width * height * cellSize) {
    return std::unexpected(error("buffer is too small"));
  }

  return {};
}

auto SimulationGrid::create(
  std::vector<std::byte>* buffer,
  unsigned int width,
  unsigned int height,
  unsigned int cellSize,
  unsigned int cellValueOffset
) -> std::expected<SimulationGrid, engine::Error> {
  auto validationResult = validateBuffer(buffer, width, height, cellSize);
  if (!validationResult.has_value()) {
    return std::unexpected(std::move(validationResult.error()));
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

auto SimulationGrid::getCell(unsigned int x, unsigned int y) -> std::byte {
  const auto& buffer = *m_buffer;

  const auto idx = bufferIndex(x, y);

  return buffer[idx];
}

auto SimulationGrid::setCell(unsigned int x, unsigned int y, std::byte value) -> void {
  auto& buffer = *m_buffer;

  const auto idx = bufferIndex(x, y);

  buffer[idx] = value;
}

auto SimulationGrid::getWidth() -> unsigned int {
  return m_width;
}

auto SimulationGrid::getHeight() -> unsigned int {
  return m_height;
}

auto SimulationGrid::swapBuffer(
  std::vector<std::byte>* buffer
) -> std::expected<void, engine::Error> {
  auto validationResult = validateBuffer(buffer, m_width, m_height, m_cellSize);
  if (!validationResult.has_value()) {
    return validationResult;
  }

  m_buffer = buffer;

  return {};
}

auto SimulationGrid::bufferIndex(unsigned int x, unsigned int y) -> unsigned int {
  return (y * m_width + x) * m_cellSize + m_cellValueOffset;
}