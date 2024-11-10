#ifndef APPLICATION_SIMULATION_ISIMULATION_HPP
#define APPLICATION_SIMULATION_ISIMULATION_HPP

#include "application/Context.hpp"
#include "application/simulation/SimulationGrid.hpp"
#include "engine/error/Error.hpp"
#include <expected>
#include <functional>

class ISimulation {
public:
  virtual ~ISimulation() {}

  virtual auto paint(const std::function<void(SimulationGrid&)>& paintFn) -> void = 0;

  virtual auto onCreate(const Context& applicationContext) -> std::expected<void, engine::Error> { return {}; }

  virtual auto onDestroy(const Context& applicationContext) -> std::expected<void, engine::Error> { return {}; }

  virtual auto onUpdate(const Context& applicationContext) -> std::expected<void, engine::Error> { return {}; }
};

#endif