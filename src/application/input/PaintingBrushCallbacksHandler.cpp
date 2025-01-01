#include "application/input/PaintingBrushCallbacksHandler.hpp"
#include "application/painting/PaintingBrush.hpp"
#include "application/simulation/cell.hpp"
#include "engine/input/binding/KeyboardKey.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <algorithm>
#include <iostream>
#include <utility>

using engine::input::KeyboardKey;
using engine::Size2D;
using std::shared_ptr;

auto PaintingBrushCallbacksHandler::create(shared_ptr<PaintingBrush> paintingBrush) -> PaintingBrushCallbacksHandler {
  paintingBrush->setValue(cell::SAND);
  paintingBrush->setSize(1);

  return PaintingBrushCallbacksHandler(std::move(paintingBrush));
}

PaintingBrushCallbacksHandler::PaintingBrushCallbacksHandler(shared_ptr<PaintingBrush> paintingBrush)
  :m_paintingBrush(std::move(paintingBrush))
{}

auto PaintingBrushCallbacksHandler::onKeyEvent(KeyboardKey key, bool pressed) -> void {
  if (!pressed) {
    return;
  }

  switch (key) {
    case KeyboardKey::_1: m_paintingBrush->setValue(2 /* AIR */);   break;
    case KeyboardKey::_2: m_paintingBrush->setValue(4 /* SAND */);  break;
    case KeyboardKey::_3: m_paintingBrush->setValue(8 /* WATER */); break;
    case KeyboardKey::PLUS: {
      auto brushSize = m_paintingBrush->getSize();
      brushSize = std::min(brushSize + 1, 50);
      std::cout << (int) brushSize << '\n';
      m_paintingBrush->setSize(brushSize);
      break;
    }
    case KeyboardKey::MINUS: {
      auto brushSize = m_paintingBrush->getSize();
      brushSize = std::max(brushSize - 1, 1);
      std::cout << (int) brushSize << '\n';
      m_paintingBrush->setSize(brushSize);
      break;
    }
    default:
      break;
  }
}

auto PaintingBrushCallbacksHandler::onSizeEvent(Size2D<uint32_t> size) -> void {
  m_paintingBrush->setFramebufferSize(size);
}