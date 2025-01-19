#ifndef APPLICATION_SIMULATION_GPU_GPU_APPLICATION_HPP
#define APPLICATION_SIMULATION_GPU_GPU_APPLICATION_HPP

#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/painting/ApplicationPainting.hpp"
#include "application/simulation/gpu/GpuSimulator.hpp"
#include "engine/EngineContext.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include <expected>

class GpuApplication :public engine::IApplication {
public:
  static auto create(
    int width,
    int height
  ) -> std::expected<GpuApplication, engine::Error>;

  GpuApplication(const GpuApplication&) = delete;
  GpuApplication(GpuApplication&&) = default;

  auto operator=(const GpuApplication&) -> GpuApplication& = delete;
  auto operator=(GpuApplication&&) -> GpuApplication& = delete;

  auto onCreate(engine::EngineContext& context) -> std::expected<void, engine::Error> override;
  auto onUpdate(engine::EngineContext& context) -> std::expected<void, engine::Error> override;

private:
  GpuApplication(uint32_t width, uint32_t height);

  auto initBuffer(bool isGpuBigEndian) -> std::expected<void, engine::Error>;
  auto initSimulation() -> std::expected<void, engine::Error>;
  auto initDrawing() -> std::expected<void, engine::Error>;
  auto initPainting(engine::EngineContext& context, bool isGpuBigEndian) -> void;

  auto paint(const engine::EngineContext& context) -> void;

  /**
   * @brief includes padding
   */
  engine::Size2D<uint32_t> m_size;
  std::vector<uint8_t> m_buffer;

  std::optional<GpuSimulator> m_simulator;

  std::optional<engine::ShaderStorageBuffer> m_inputSSBO;
  std::optional<engine::ShaderStorageBuffer> m_outputSSBO;

  std::optional<engine::Texture> m_drawingTexture;
  std::optional<TextureDrawingProgram> m_drawingProgram;

  std::optional<ApplicationPainting> m_applicationPainting;
};

#endif