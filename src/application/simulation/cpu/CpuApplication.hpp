#ifndef APPLICATION_SIMULATION_CPU_CPU_APPLICATION_HPP
#define APPLICATION_SIMULATION_CPU_CPU_APPLICATION_HPP

#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/painting/PaintingBrush.hpp"
#include "application/simulation/Cell.hpp"
#include "application/simulation/cpu/CpuSimulator.hpp"
#include "application/simulation/input/SimulationInputHandler.hpp"
#include "engine/EngineContext.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include <memory>

class CpuApplication :public engine::IApplication {
public:
  static auto create(
    int width,
    int height
  ) -> std::expected<CpuApplication, engine::Error>;

  CpuApplication(const CpuApplication&) = delete;
  CpuApplication(CpuApplication&&) = default;

  auto operator=(const CpuApplication&) -> CpuApplication& = delete;
  auto operator=(CpuApplication&&) -> CpuApplication& = delete;

  auto onCreate(engine::EngineContext& applicationContext) -> std::expected<void, engine::Error> override;
  auto onUpdate(engine::EngineContext& applicationContext) -> std::expected<void, engine::Error> override;

private:
  CpuApplication(
    CpuSimulator&& simulator,
    int width,
    int height
  );

  auto initDrawing() -> std::expected<void, engine::Error>;
  auto initSimulation() -> void;
  auto initPainting() -> void;
  auto initKeyboardCallback(engine::EngineContext& context) -> void;

  CpuSimulator m_simulator;
  int m_width;
  int m_height;
  std::vector<Cell> m_bufferIn;
  std::vector<Cell> m_bufferOut;
  std::vector<GLfloat> m_textureBuffer;

  std::optional<engine::Texture> m_drawingTexture;
  std::optional<TextureDrawingProgram> m_drawingProgram;

  std::optional<std::shared_ptr<SimulationInputHandler>> m_inputHandler;
  std::optional<std::shared_ptr<PaintingBrush>> m_paintingBrush;
};

#endif