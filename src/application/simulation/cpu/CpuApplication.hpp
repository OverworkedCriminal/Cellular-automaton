#ifndef APPLICATION_SIMULATION_CPU_CPU_APPLICATION_HPP
#define APPLICATION_SIMULATION_CPU_CPU_APPLICATION_HPP

#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/painting/ApplicationPainting.hpp"
#include "application/simulation/cpu/CpuSimulator.hpp"
#include "engine/EngineContext.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <cstdint>

class CpuApplication :public engine::IApplication {
public:
  static auto create(
    int width,
    int height,
    uint32_t processorsCount
  ) -> std::expected<CpuApplication, engine::Error>;

  CpuApplication(const CpuApplication&) = delete;
  CpuApplication(CpuApplication&&) = default;

  auto operator=(const CpuApplication&) -> CpuApplication& = delete;
  auto operator=(CpuApplication&&) -> CpuApplication& = delete;

  auto onCreate(engine::EngineContext& context) -> std::expected<void, engine::Error> override;
  auto onUpdate(engine::EngineContext& context) -> std::expected<void, engine::Error> override;

private:
  CpuApplication(
    CpuSimulator&& simulator,
    uint32_t width,
    uint32_t height
  );

  auto initDrawing() -> std::expected<void, engine::Error>;
  auto initSimulation() -> void;
  auto initPainting(engine::EngineContext& context) -> void;

  auto paint(const engine::EngineContext& context) -> void;

  /**
   * @brief includes padding
   */
  engine::Size2D<uint32_t> m_size;
  CpuSimulator m_simulator;

  std::vector<uint8_t> m_bufferIn;
  std::vector<uint8_t> m_bufferOut;
  std::vector<GLfloat> m_textureBuffer;

  std::optional<engine::Texture> m_drawingTexture;
  std::optional<TextureDrawingProgram> m_drawingProgram;

  std::optional<ApplicationPainting> m_applicationPainting;
};

#endif