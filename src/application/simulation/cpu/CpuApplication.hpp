#ifndef APPLICATION_SIMULATION_CPU_CPU_APPLICATION_HPP
#define APPLICATION_SIMULATION_CPU_CPU_APPLICATION_HPP

#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/painting/PaintingBrush.hpp"
#include "application/simulation/Cell.hpp"
#include "application/simulation/cpu/CpuSimulator.hpp"
#include "engine/EngineContext.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/input/callback/IKeyboardKeyCallback.hpp"
#include <memory>

class CpuSimulation :public engine::IApplication {
public:
  static auto create(
    int width,
    int height
  ) -> std::expected<CpuSimulation, engine::Error>;

  CpuSimulation(const CpuSimulation&) = delete;
  CpuSimulation(CpuSimulation&&) = default;

  auto operator=(const CpuSimulation&) -> CpuSimulation& = delete;
  auto operator=(CpuSimulation&&) -> CpuSimulation& = delete;

  auto onCreate(engine::EngineContext& applicationContext) -> std::expected<void, engine::Error> override;
  auto onUpdate(engine::EngineContext& applicationContext) -> std::expected<void, engine::Error> override;

private:
  CpuSimulation(
    CpuSimulator&& simulator,
    int width,
    int height
  );

  auto initDrawing() -> std::expected<void, engine::Error>;
  auto initSimulation() -> void;
  auto initPainting() -> void;
  auto initKeyboardCallback(engine::EngineContext& context) -> void;

  auto onKeyboardKeyEvent(engine::input::KeyboardKey key, bool pressed) -> void;

  CpuSimulator m_simulator;
  int m_width;
  int m_height;
  std::vector<Cell> m_bufferIn;
  std::vector<Cell> m_bufferOut;
  std::vector<GLfloat> m_textureBuffer;

  std::optional<engine::Texture> m_drawingTexture;
  std::optional<TextureDrawingProgram> m_drawingProgram;

  std::optional<std::shared_ptr<engine::input::IKeyboardKeyCallback>> m_keyboardCallback;
  std::optional<PaintingBrush> m_paintingBrush;
};

#endif