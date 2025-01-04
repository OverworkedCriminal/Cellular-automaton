#include "application/painting/ApplicationPainting.hpp"
#include "application/painting/ApplicationPaintingCallback.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/brush/IPaintingBrush.hpp"
#include "application/painting/brush/PaintingBrushDescription.hpp"
#include "application/painting/brush/SquarePaintingBrush.hpp"
#include "application/simulation/cell.hpp"
#include "engine/EngineContext.hpp"
#include "engine/callback/IKeyboardKeyCallback.hpp"
#include <memory>
#include <utility>

using std::vector;
using std::make_unique;
using std::make_shared;
using std::shared_ptr;
using std::unique_ptr;
using engine::EngineContext;
using engine::IKeyboardKeyCallback;

/**
 * @brief Maps position from window space to simulation space
 * 
 * @param windowPosition 
 * @param framebufferSize 
 * @param simulationSize (including padding added to each side of the simulation canvas)
 * 
 * @return engine::Position2D<uint32_t> 
 */
static auto mapWindowPositionToSimulationPosition(
  engine::Position2D<uint32_t> windowPosition,
  engine::Size2D<uint32_t> framebufferSize,
  engine::Size2D<uint32_t> simulationSize
) -> engine::Position2D<uint32_t> {
  const auto [posX, posY] = windowPosition;
  const auto [framebufferWidth, framebufferHeight] = framebufferSize;
  const auto [simulationWidth, simulationHeight] = simulationSize;

  const float scaleWidth = static_cast<float>(simulationWidth) / framebufferWidth;
  const float scaleHeight = static_cast<float>(simulationHeight) / framebufferHeight;

  const uint32_t mappedPosX = scaleWidth * posX;
  const uint32_t mappedPosY = scaleHeight * posY;

  return {
    .x = mappedPosX,
    .y = mappedPosY
  };
}

auto ApplicationPainting::create(
  engine::EngineContext& context,
  PaintingCanvasDescription canvasDescription
) -> ApplicationPainting {
  auto brushDescriptionPtr = make_shared<PaintingBrushDescription>(
    PaintingBrushDescription {
      .size = 1,
      .cell = cell::SAND
    }
  );
  auto paintingBrushPtr = make_unique<SquarePaintingBrush>(SquarePaintingBrush::create());
  auto keyboardCallback = make_shared<ApplicationPaintingCallback>(
    ApplicationPaintingCallback::create(brushDescriptionPtr)
  );

  context
    .inputSystem
    .addKeyboardKeyCallback(keyboardCallback);

  return ApplicationPainting(
    canvasDescription,
    std::move(brushDescriptionPtr),
    std::move(paintingBrushPtr),
    std::move(keyboardCallback)
  );
}

ApplicationPainting::ApplicationPainting(
  PaintingCanvasDescription canvasDescription,
  shared_ptr<PaintingBrushDescription>&& brushDescriptionPtr,
  unique_ptr<IPaintingBrush>&& paintingBrushPtr,
  shared_ptr<IKeyboardKeyCallback>&& keyboardCallback
)
  :m_canvasDescription(canvasDescription)
  ,m_brushDescriptionPtr(std::move(brushDescriptionPtr))
  ,m_paintingBrushPtr(std::move(paintingBrushPtr))
  ,m_keyboardCallback(std::move(keyboardCallback))
{}

auto ApplicationPainting::paint(
  const EngineContext& context,
  vector<uint8_t>& canvas
) const -> void {
  const auto position = mapWindowPositionToSimulationPosition(
    context.inputSystem.getMousePosition(),
    context.windowSystem.getFramebufferSize(),
    m_canvasDescription.size
  );

  m_paintingBrushPtr->paint(
    *m_brushDescriptionPtr,
    position,
    m_canvasDescription,
    canvas
  );
}