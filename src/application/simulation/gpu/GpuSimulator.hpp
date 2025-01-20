#ifndef APPLICATION_SIMULATION_GPU_GPU_SIMULATOR_HPP
#define APPLICATION_SIMULATION_GPU_GPU_SIMULATOR_HPP

#include "engine/error/Error.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <expected>

class GpuSimulator {
public:
  /**
   * @brief Class constructor
   * 
   * @param size (does not include padding)
   *
   * @return std::expected<GpuSimulator, engine::Error> 
   *
   * @throws engine::Error when width or height is less than 1 or OpenGL error occurs
   */
  static auto create(
    engine::Size2D<uint32_t> size
  ) -> std::expected<GpuSimulator, engine::Error>;

  GpuSimulator(const GpuSimulator&) = delete;
  GpuSimulator(GpuSimulator&&) = default;

  auto operator=(const GpuSimulator&) -> GpuSimulator& = delete;
  auto operator=(GpuSimulator&&) -> GpuSimulator& = default;

  auto run(
    engine::ShaderStorageBuffer& input,
    engine::ShaderStorageBuffer& output,
    engine::Texture& outputTexture
  ) -> std::expected<void, engine::Error>;

private:
  GpuSimulator(
    engine::Program&& program,
    engine::Size2D<GLuint> computeSpace
  );

  engine::Program m_program;
  engine::Size2D<GLuint> m_computeSpace;

  GLint m_priorityDirection;
};

#endif