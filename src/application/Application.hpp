#ifndef APPLICATION_APPLICATION_HPP
#define APPLICATION_APPLICATION_HPP

#include "application/simulation/ISimulation.hpp"
#include "application/simulation/SimulationGrid.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/graphics/buffer/VertexArrayObject.hpp"
#include "engine/graphics/buffer/VertexBuffer.hpp"
#include "engine/graphics/shader/Program.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include <memory>

class Application :public engine::IApplication {
public:
  static auto create(
    int width,
    int height,
    std::unique_ptr<ISimulation> simulation
  ) -> std::expected<Application, engine::Error>;

  Application(const Application&) = delete;
  Application(Application&&) = default;

  auto operator=(const Application&) -> Application& = delete;
  auto operator=(Application&&) -> Application& = default;

  auto onCreate() -> std::expected<void, engine::Error> override;
  auto onDestroy() -> std::expected<void, engine::Error> override;
  auto onUpdate() -> std::expected<void, engine::Error> override;

  auto onKeyboardInput(engine::KeyboardKey key, bool pressed) -> void override;
  auto onMouseMoveInput(unsigned int posX, unsigned int posY) -> void override;
  auto onMouseButtonInput(engine::MouseButton button, bool pressed) -> void override;

  auto onFramebufferSizeChange(unsigned int width, unsigned int height) -> void override;

private:
  Application(
    int width,
    int height,
    std::unique_ptr<ISimulation>&& simulation
  );

  auto initDrawing() -> std::expected<void, engine::Error>;
  auto updateContext() -> void;

  auto paintSquare(SimulationGrid& simulationGrid) -> void;

  int m_width;
  int m_height;

  unsigned int m_mousePosX;
  unsigned int m_mousePosY;
  bool m_mouseLeftPressed;

  float m_simulationWidthScale;
  float m_simulationHeightScale;

  std::unique_ptr<ISimulation> m_simulation;

  std::function<void(SimulationGrid&)> m_paintFn;

  std::optional<engine::VertexArrayObject> m_vaoOpt;
  std::optional<engine::VertexBuffer> m_vboOpt;

  std::optional<engine::Program> m_drawingProgramOpt;
  std::optional<std::shared_ptr<engine::Texture>> m_texturePtrOpt;

  Context m_context;

  std::byte m_selectedCellValue;
};

#endif