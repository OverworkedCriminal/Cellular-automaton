#ifndef APPLICATION_SIMULATION_INPUT_SIMULATION_INPUT_HANDLER_HPP
#define APPLICATION_SIMULATION_INPUT_SIMULATION_INPUT_HANDLER_HPP

#include "application/painting/PaintingBrush.hpp"
#include "engine/input/callback/IKeyboardKeyCallback.hpp"
#include <memory>

class SimulationInputHandler :public engine::input::IKeyboardKeyCallback {
public:
  static auto create(std::shared_ptr<PaintingBrush> paintingBrush) -> SimulationInputHandler;

  SimulationInputHandler(const SimulationInputHandler&) = delete;
  SimulationInputHandler(SimulationInputHandler&&) = default;

  auto operator=(const SimulationInputHandler&) -> SimulationInputHandler& = delete;
  auto operator=(SimulationInputHandler&&) -> SimulationInputHandler& = default;

  auto onKeyEvent(engine::input::KeyboardKey key, bool pressed) -> void override;

private:
  SimulationInputHandler(std::shared_ptr<PaintingBrush> paintingBrush);

  std::shared_ptr<PaintingBrush> m_paintingBrush;
};

#endif