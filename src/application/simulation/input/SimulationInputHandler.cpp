#include "application/simulation/input/SimulationInputHandler.hpp"
#include "application/painting/PaintingBrush.hpp"
#include "engine/input/binding/KeyboardKey.hpp"
#include <utility>

using engine::input::KeyboardKey;
using std::shared_ptr;

auto SimulationInputHandler::create(shared_ptr<PaintingBrush> paintingBrush) -> SimulationInputHandler {
  return SimulationInputHandler(std::move(paintingBrush));
}

SimulationInputHandler::SimulationInputHandler(shared_ptr<PaintingBrush> paintingBrush)
  :m_paintingBrush(std::move(paintingBrush))
{}

auto SimulationInputHandler::onKeyEvent(KeyboardKey key, bool pressed) -> void {
  if (!pressed) {
    return;
  }

  switch (key) {
    case KeyboardKey::_1: m_paintingBrush->setValue(2 /* AIR */);   break;
    case KeyboardKey::_2: m_paintingBrush->setValue(4 /* SAND */);  break;
    case KeyboardKey::_3: m_paintingBrush->setValue(8 /* WATER */); break;
    default:
      break;
  }
}