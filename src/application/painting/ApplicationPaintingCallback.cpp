#include "application/painting/ApplicationPaintingCallback.hpp"
#include "engine/input/binding/KeyboardKey.hpp"

using engine::input::KeyboardKey;

auto ApplicationPaintingCallback::create(
  std::shared_ptr<PaintingBrushDescription> paintingDescriptionPtr
) -> ApplicationPaintingCallback {
  return ApplicationPaintingCallback(std::move(paintingDescriptionPtr));
}

ApplicationPaintingCallback::ApplicationPaintingCallback(
  std::shared_ptr<PaintingBrushDescription>&& brushDescriptionPtr
)
  :m_brushDescriptionPtr(brushDescriptionPtr)
{}

auto ApplicationPaintingCallback::onKeyEvent(KeyboardKey key, bool pressed) -> void {
  if (!pressed) {
    return;
  }

  switch (key) {
    case KeyboardKey::_1: m_brushDescriptionPtr->cell = 2;  break; // AIR
    case KeyboardKey::_2: m_brushDescriptionPtr->cell = 4;  break; // SAND
    case KeyboardKey::_3: m_brushDescriptionPtr->cell = 8;  break; // WATER
    case KeyboardKey::_4: m_brushDescriptionPtr->cell = 32; break; // SMOKE
    default:
      break;
  }
}

auto ApplicationPaintingCallback::onScrollEvent(
  double offsetX [[maybe_unused]],
  double offsetY
) -> void {
  if (offsetY > 0) {
    m_brushDescriptionPtr->size = std::min(m_brushDescriptionPtr->size + 1, 50);
  } else if (offsetY < 0 ) {
    m_brushDescriptionPtr->size = std::max(m_brushDescriptionPtr->size - 1, 1);
  }
}
