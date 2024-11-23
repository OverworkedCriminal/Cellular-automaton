#ifndef APPLICATION_APPLICATION_HPP
#define APPLICATION_APPLICATION_HPP

#include "application/simulation/ISimulation.hpp"
#include "engine/application/IApplication.hpp"
#include <memory>

class Application :public engine::IApplication {
public:
  static auto create(std::unique_ptr<ISimulation> simulation) -> Application;

  Application(const Application&) = delete;
  Application(Application&&) = default;

  auto operator=(const Application&) -> Application& = delete;
  auto operator=(Application&&) -> Application& = default;

  auto onCreate() -> std::expected<void, engine::Error> override;
  auto onDestroy() -> std::expected<void, engine::Error> override;
  auto onUpdate() -> std::expected<void, engine::Error> override;

  auto onKeyboardInput(engine::KeyboardKey key, bool pressed) -> void override;
  auto onMouseMoveInput(int posX, int posY) -> void override;
  auto onMouseButtonInput(engine::MouseButton button, bool pressed) -> void override;

  auto onFramebufferSizeChange(unsigned int width, unsigned int height) -> void override;

private:
  Application(std::unique_ptr<ISimulation>&& simulation);

  Context m_context;
  std::unique_ptr<ISimulation> m_simulation;
};

#endif