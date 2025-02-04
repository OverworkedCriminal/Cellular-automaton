#ifndef APPLICATION_PAINTING_APPLICATION_PAINTING_CALLBACK_HPP
#define APPLICATION_PAINTING_APPLICATION_PAINTING_CALLBACK_HPP

#include "application/painting/brush/PaintingBrushDescription.hpp"
#include "engine/callback/IKeyboardKeyCallback.hpp"
#include "engine/callback/IMouseScrollCallback.hpp"
#include <memory>

class ApplicationPaintingCallback
  :public engine::IKeyboardKeyCallback 
  ,public engine::IMouseScrollCallback
{
public:
  static auto create(
    std::shared_ptr<PaintingBrushDescription>
  ) -> ApplicationPaintingCallback;
  
  ApplicationPaintingCallback(const ApplicationPaintingCallback&) = delete;
  ApplicationPaintingCallback(ApplicationPaintingCallback&&) = default;

  auto operator=(const ApplicationPaintingCallback&) -> ApplicationPaintingCallback& = delete;
  auto operator=(ApplicationPaintingCallback&&) -> ApplicationPaintingCallback& = default;

  auto onKeyEvent(engine::input::KeyboardKey key, bool pressed) -> void override;
  auto onScrollEvent(double offsetX, double offsetY) -> void override;

private:
  ApplicationPaintingCallback(
    std::shared_ptr<PaintingBrushDescription>&& brushDescriptionPtr
  );

  std::shared_ptr<PaintingBrushDescription> m_brushDescriptionPtr;
};

#endif