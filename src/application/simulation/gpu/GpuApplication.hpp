#ifndef APPLICATION_SIMULATION_GPU_GPU_APPLICATION_HPP
#define APPLICATION_SIMULATION_GPU_GPU_APPLICATION_HPP

#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/painting/ApplicationPainting.hpp"
#include "application/simulation/cell.hpp"
#include "application/simulation/gpu/GpuSimulator.hpp"
#include "engine/EngineContext.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <expected>

class GpuApplication :public engine::IApplication {
public:

  /**
   * @brief Application constructor
   * 
   * @param size (does not include padding)
   * @return std::expected<GpuApplication, engine::Error> 
   */
  static auto create(
    engine::Size2D<uint32_t> size
  ) -> std::expected<GpuApplication, engine::Error>;

  GpuApplication(const GpuApplication&) = delete;
  GpuApplication(GpuApplication&&) = default;

  auto operator=(const GpuApplication&) -> GpuApplication& = delete;
  auto operator=(GpuApplication&&) -> GpuApplication& = delete;

  auto onCreate(engine::EngineContext& context) -> std::expected<void, engine::Error> override;
  auto onUpdate(engine::EngineContext& context) -> std::expected<void, engine::Error> override;

private:
  GpuApplication(engine::Size2D<uint32_t> sizeWithPadding);

  auto initBuffer() -> std::expected<void, engine::Error>;
  auto initSimulation() -> std::expected<void, engine::Error>;
  auto initDrawing() -> std::expected<void, engine::Error>;
  auto initPainting(engine::EngineContext& context) -> void;

  auto paint(const engine::EngineContext& context) -> void;

  engine::Size2D<uint32_t> m_sizeWithPadding;
  std::vector<cell_t> m_buffer;

  std::optional<GpuSimulator> m_simulator;

  std::optional<engine::ShaderStorageBuffer> m_inputSSBO;
  std::optional<engine::ShaderStorageBuffer> m_outputSSBO;

  std::optional<engine::Texture> m_drawingTexture;
  std::optional<TextureDrawingProgram> m_drawingProgram;

  std::optional<ApplicationPainting> m_applicationPainting;

  bool m_isCpuAndGpuEndianessMatching;
};

#endif