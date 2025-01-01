#ifndef APPLICATION_SIMULATION_GPU_GPU_APPLICATION_HPP
#define APPLICATION_SIMULATION_GPU_GPU_APPLICATION_HPP

#include "application/drawing/TextureDrawingProgram.hpp"
#include "application/input/PaintingBrushCallbacksHandler.hpp"
#include "application/painting/PaintingBrush.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "engine/EngineContext.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/error/Error.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include <expected>
#include <memory>

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

  auto initBuffer() -> std::expected<void, engine::Error>;
  auto initSimulation() -> std::expected<void, engine::Error>;
  auto initDrawing() -> std::expected<void, engine::Error>;
  auto initPainting(engine::EngineContext& context) -> void;

  auto paint(const engine::EngineContext& context) -> void;

  std::vector<uint8_t> m_buffer;

  std::optional<engine::Program> m_simulationProgram;
  std::optional<engine::ShaderStorageBuffer> m_inputSSBO;
  std::optional<engine::ShaderStorageBuffer> m_outputSSBO;
  int m_computeSpaceX;
  int m_computeSpaceY;

  std::optional<engine::Texture> m_drawingTexture;
  std::optional<TextureDrawingProgram> m_drawingProgram;

  std::optional<std::shared_ptr<PaintingBrushCallbacksHandler>> m_inputHandler;

  std::optional<std::shared_ptr<PaintingBrush>> m_paintingBrush;
  PaintingCanvasDescription m_canvasDescription;
};

#endif