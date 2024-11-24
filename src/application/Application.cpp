#include "application/Application.hpp"
#include "engine/utils/error.hpp"
#include <algorithm>

using engine::error;

auto Application::create(std::unique_ptr<ISimulation> simulation) -> Application {
  return Application(std::move(simulation));
}

Application::Application(std::unique_ptr<ISimulation>&& simulation)
  :m_simulation(std::forward<std::unique_ptr<ISimulation>&&>(simulation))
{}

auto Application::onCreate() -> std::expected<void, engine::Error> {
  m_context.mouseLeftPressed = false;

  auto simulationResult = m_simulation->onCreate(m_context);
  if (!simulationResult.has_value()) {
    return std::unexpected(error("simulation onCreate failed", simulationResult.error()));
  }

  return {};
}

auto Application::onDestroy() -> std::expected<void, engine::Error> {
  auto simulationResult = m_simulation->onDestroy(m_context);
  if (!simulationResult.has_value()) {
    return std::unexpected(error("simulation onDestroy failed", simulationResult.error()));
  }

  return {};
}

auto Application::onUpdate() -> std::expected<void, engine::Error> {
  auto simulationResult = m_simulation->onUpdate(m_context);
  if (!simulationResult.has_value()) {
    return std::unexpected(error("simulation onUpdate failed", simulationResult.error()));
  }

  return {};
}

auto Application::onKeyboardInput(engine::KeyboardKey key, bool pressed) -> void {
  if (pressed == false) {
    return;
  }

  m_context.keyboardLastKeyPressed = key;
}

auto Application::onMouseMoveInput(int posX, int posY) -> void {
  m_context.mousePosX = std::clamp(posX, 0, static_cast<int>(m_context.framebufferWidth));
  m_context.mousePosY = std::clamp(posY, 0, static_cast<int>(m_context.framebufferHeight));
}

auto Application::onMouseButtonInput(engine::MouseButton button, bool pressed) -> void {
  if (button != engine::MouseButton::LEFT) {
    return;
  }

  m_context.mouseLeftPressed = pressed;
}

auto Application::onFramebufferSizeChange(unsigned int width, unsigned int height) -> void {
  m_context.framebufferWidth = width;
  m_context.framebufferHeight = height;
}

