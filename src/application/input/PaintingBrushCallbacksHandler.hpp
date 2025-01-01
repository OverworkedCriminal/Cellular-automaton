#ifndef APPLICATION_INPUT_PAINTING_BRUSH_CALLBACKS_HANDLER_HPP
#define APPLICATION_INPUT_PAINTING_BRUSH_CALLBACKS_HANDLER_HPP

#include "application/painting/PaintingBrush.hpp"
#include "engine/callback/IKeyboardKeyCallback.hpp"
#include "engine/callback/IFramebufferSizeCallback.hpp"
#include <memory>

class PaintingBrushCallbacksHandler
  :public engine::IKeyboardKeyCallback
  ,public engine::IFramebufferSizeCallback 
{
public:
  static auto create(std::shared_ptr<PaintingBrush> paintingBrush) -> PaintingBrushCallbacksHandler;

  PaintingBrushCallbacksHandler(const PaintingBrushCallbacksHandler&) = delete;
  PaintingBrushCallbacksHandler(PaintingBrushCallbacksHandler&&) = default;

  auto operator=(const PaintingBrushCallbacksHandler&) -> PaintingBrushCallbacksHandler& = delete;
  auto operator=(PaintingBrushCallbacksHandler&&) -> PaintingBrushCallbacksHandler& = default;

  auto onKeyEvent(engine::input::KeyboardKey key, bool pressed) -> void override;
  auto onSizeEvent(engine::Size2D<uint32_t> size) -> void override;

private:
  PaintingBrushCallbacksHandler(std::shared_ptr<PaintingBrush> paintingBrush);

  std::shared_ptr<PaintingBrush> m_paintingBrush;
};

#endif