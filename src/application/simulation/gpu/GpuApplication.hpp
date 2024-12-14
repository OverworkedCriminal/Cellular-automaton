#ifndef APPLICATION_SIMULATION_GPU_GPU_APPLICATION_HPP
#define APPLICATION_SIMULATION_GPU_GPU_APPLICATION_HPP

#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/painting/PaintingBrush.hpp"
#include "application/simulation/Cell.hpp"
#include "engine/EngineContext.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/input/binding/KeyboardKey.hpp"
#include "engine/input/callback/IKeyboardKeyCallback.hpp"
#include <expected>
#include <memory>

class GpuSimulation :public engine::IApplication {
public:
  static auto create(
    int width,
    int height
  ) -> std::expected<GpuSimulation, engine::Error>;

  GpuSimulation(const GpuSimulation&) = delete;
  GpuSimulation(GpuSimulation&&) = default;

  auto operator=(const GpuSimulation&) -> GpuSimulation& = delete;
  auto operator=(GpuSimulation&&) -> GpuSimulation& = delete;

  auto onCreate(engine::EngineContext& context) -> std::expected<void, engine::Error> override;
  auto onUpdate(engine::EngineContext& context) -> std::expected<void, engine::Error> override;

private:
  GpuSimulation(int width, int height);

  auto initBuffer() -> std::expected<void, engine::Error>;
  auto initSimulation() -> std::expected<void, engine::Error>;
  auto initDrawing() -> std::expected<void, engine::Error>;
  auto initPainting() -> void;
  auto initKeyboardCallback(engine::EngineContext& context) -> void;

  auto paint(const engine::EngineContext& context) -> void;

  auto onKeyboardKeyEvent(engine::input::KeyboardKey key, bool pressed) -> void;

  unsigned int m_width;
  unsigned int m_height;

  std::optional<std::vector<Cell>> m_buffer;
  unsigned int m_bufferValueOffset;
  unsigned int m_bufferValueStride;

  std::optional<engine::Program> m_simulationProgram;
  std::optional<engine::ShaderStorageBuffer> m_inputSSBO;
  std::optional<engine::ShaderStorageBuffer> m_outputSSBO;
  int m_computeSpaceX;
  int m_computeSpaceY;

  std::optional<engine::Texture> m_drawingTexture;
  std::optional<TextureDrawingProgram> m_drawingProgram;

  std::optional<std::shared_ptr<engine::input::IKeyboardKeyCallback>> m_keyboardCallback;
  std::optional<PaintingBrush> m_paintingBrush;
};

#endif