#ifndef APPLICATION_PAINTING_APPLICATION_PAINTING_HPP
#define APPLICATION_PAINTING_APPLICATION_PAINTING_HPP

#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/brush/IPaintingBrush.hpp"
#include "application/painting/brush/PaintingBrushDescription.hpp"
#include "engine/EngineContext.hpp"
#include "engine/callback/IKeyboardKeyCallback.hpp"
#include <vector>

class ApplicationPainting {
public:
  static auto create(
    engine::EngineContext& context,
    PaintingCanvasDescription canvasDescription
  ) -> ApplicationPainting;

  ApplicationPainting(const ApplicationPainting&) = delete;
  ApplicationPainting(ApplicationPainting&&) = default;

  auto operator=(const ApplicationPainting&) -> ApplicationPainting& = delete;
  auto operator=(ApplicationPainting&&) -> ApplicationPainting& = default;

  auto paint(const engine::EngineContext& context, std::vector<uint8_t>& canvas) const -> void;

private:
  ApplicationPainting(
    PaintingCanvasDescription canvasDescription,
    std::shared_ptr<PaintingBrushDescription>&& brushDescriptionPtr,
    std::unique_ptr<IPaintingBrush>&& paintingBrushPtr,
    std::shared_ptr<engine::IKeyboardKeyCallback>&& keyboardCallback
  );

  PaintingCanvasDescription m_canvasDescription;
  std::shared_ptr<PaintingBrushDescription> m_brushDescriptionPtr;

  std::unique_ptr<IPaintingBrush> m_paintingBrushPtr;

  std::shared_ptr<engine::IKeyboardKeyCallback> m_keyboardCallback;
};

#endif