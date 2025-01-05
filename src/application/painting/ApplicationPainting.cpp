#include "application/painting/ApplicationPainting.hpp"
#include "application/painting/ApplicationPaintingCallback.hpp"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/brush/IPaintingBrush.hpp"
#include "application/painting/brush/PaintingBrushDescription.hpp"
#include "application/painting/brush/SquarePaintingBrush.hpp"
#include "application/painting/utils/position_mapping.hpp"
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