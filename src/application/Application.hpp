#ifndef APPLICATION_APPLICATION_HPP
#define APPLICATION_APPLICATION_HPP

#include "application/simulation/ISimulation.hpp"
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

  auto onCreate(
    const engine::Context& context
  ) -> std::expected<void, engine::Error> override;

  auto onDestroy(
    const engine::Context& context
  ) -> std::expected<void, engine::Error> override;

  auto onUpdate(
    const engine::Context& context
  ) -> std::expected<void, engine::Error> override;

private:
  Application(
    int width,
    int height,
    std::unique_ptr<ISimulation>&& simulation
  );

  auto initDrawing() -> std::expected<void, engine::Error>;
  auto updateSelectedCellValue(const engine::Context& context) -> void;
  auto updateSimulationGrid(const engine::Context& context) -> void;
  auto updateContext() -> void;

  int m_width;
  int m_height;

  std::unique_ptr<ISimulation> m_simulation;

  std::optional<engine::VertexArrayObject> m_vaoOpt;
  std::optional<engine::VertexBuffer> m_vboOpt;

  std::optional<engine::Program> m_drawingProgramOpt;
  std::optional<std::shared_ptr<engine::Texture>> m_texturePtrOpt;

  Context m_context;

  std::byte m_selectedCellValue;
};

#endif