#ifndef APPLICATION_SIMULATION_TEST_GPU_SIMULATOR_FIXTURE_TEST_HPP
#define APPLICATION_SIMULATION_TEST_GPU_SIMULATOR_FIXTURE_TEST_HPP

#include "application/simulation/gpu/GpuSimulator.hpp"
#include "application/simulation/gpu/utils.hpp"
#include "application/simulation/test/SimulatorFixture.test.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

class TestFixture :public SimulatorFixture {
protected:
  std::optional<GpuSimulator> gpuSimulator;

  std::optional<engine::ShaderStorageBuffer> inputSSBO;
  std::optional<engine::ShaderStorageBuffer> outputSSBO;
  std::optional<engine::Texture> texture;

  TestFixture() {
    initOpenGL();

    auto isCpuGpuEndianessMatchingResult = isCpuAndGpuEndianessMatching();
    if (!isCpuGpuEndianessMatchingResult.has_value()) {
      throw std::runtime_error(isCpuGpuEndianessMatchingResult.error().message());
    }
    isEndianessMatching = *isCpuGpuEndianessMatchingResult;

    canvasDescription = {
      .size = {
        .width = 5 + 2 * PADDING_SIZE,
        .height = 5 + 2 * PADDING_SIZE
      },
      .paddingSize = PADDING_SIZE
    };

    input = std::vector<cell_t>(canvasDescription.size.width * canvasDescription.size.height, 0);
    output = std::vector<cell_t>(canvasDescription.size.width * canvasDescription.size.height, 0);

    resetBuffer(input);
    resetBuffer(output);

    auto simulatorResult = GpuSimulator::create({ .width = 5, .height = 5 });
    if (!simulatorResult.has_value()) {
      throw std::runtime_error(simulatorResult.error().message());
    }
    gpuSimulator = std::move(*simulatorResult);

    auto inputSSBOResult = engine::ShaderStorageBuffer::create(input.size() * sizeof(cell_t));
    if (!inputSSBOResult.has_value()) {
      throw std::runtime_error(inputSSBOResult.error().message());
    }
    inputSSBO = std::move(*inputSSBOResult);

    auto outputSSBOResult = engine::ShaderStorageBuffer::create(output.size() * sizeof(cell_t));
    if (!outputSSBOResult.has_value()) {
      throw std::runtime_error(outputSSBOResult.error().message());
    }
    outputSSBO = std::move(*outputSSBOResult);

    auto textureResult = engine::Texture::create(canvasDescription.size.width, canvasDescription.size.height);
    if (!textureResult.has_value()) {
      throw std::runtime_error(textureResult.error().message());
    }
    texture = std::move(*textureResult);
  }

  ~TestFixture() {
    // Overwrite OpenGL objects with nulls so they are freed before deinitOpenGL
    gpuSimulator = std::nullopt;
    inputSSBO = std::nullopt;
    outputSSBO = std::nullopt;
    texture = std::nullopt;

    deinitOpenGL();
  }

  auto runSimulator() -> void override {
    if (!isEndianessMatching) {
      switchEndianess(input);
      switchEndianess(output);
    }
    inputSSBO->store(input);
    outputSSBO->store(output);

    auto runResult = gpuSimulator->run(
      *inputSSBO,
      *outputSSBO,
      *texture
    );
    if (!runResult.has_value()) {
      throw std::runtime_error(runResult.error().message());
    }

    outputSSBO->load(output);
    if (!isEndianessMatching) {
      switchEndianess(input);
      switchEndianess(output);
    }
  }

private:
  bool isEndianessMatching;

  auto initOpenGL() -> void {
    if (!glfwInit()) {
      throw std::runtime_error("failed to init GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
      800,
      600,
      "gpu-simulator-test",
      nullptr,
      nullptr
    );
    if (!window) {
      glfwTerminate();
      throw std::runtime_error("failed to create window");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
      glfwTerminate();
      throw std::runtime_error("failed to init GLAD");
    }
  }

  auto deinitOpenGL() -> void {
    glfwTerminate();
  }

};

TEST_CASE("Invalid width", "[constructor]") {
  const auto simulator = GpuSimulator::create({ .width = 0, .height = 1 });
  REQUIRE_FALSE(simulator.has_value());
}

TEST_CASE("Invalid height", "[constructor]") {
  const auto simulator = GpuSimulator::create({ .width = 1, .height = 0 });
  REQUIRE_FALSE(simulator.has_value());
}

#endif