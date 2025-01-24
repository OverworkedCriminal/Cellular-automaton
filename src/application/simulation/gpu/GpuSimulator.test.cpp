#include "application/simulation/gpu/GpuSimulator.hpp"
#include "GLFW/glfw3.h"
#include "application/painting/PaintingCanvasDescription.hpp"
#include "application/painting/utils/position_mapping.hpp"
#include "application/simulation/cell.hpp"
#include "application/simulation/gpu/utils.hpp"
#include "application/simulation/padding.hpp"
#include "engine/graphics/shader/ShaderStorageBuffer.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include "engine/utils/dto/Position2D.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <bitset>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <stdexcept>
#include <vector>

using engine::Position2D;
using engine::ShaderStorageBuffer;
using engine::Texture;

TEST_CASE("Invalid width", "[constructor]") {
  const auto simulator = GpuSimulator::create({ .width = 0, .height = 1 });
  REQUIRE_FALSE(simulator.has_value());
}

TEST_CASE("Invalid height", "[constructor]") {
  const auto simulator = GpuSimulator::create({ .width = 1, .height = 0 });
  REQUIRE_FALSE(simulator.has_value());
}

struct Expectation {
  Position2D<int32_t> position;
  uint8_t cellMask;
};

struct Error {
  Position2D<int32_t> position;
  uint8_t cell;
  uint8_t expectedMask;
};

class TestFixture {
public:
  TestFixture() {
    initOpenGL();

    auto isGpuBigEndianResult = isGpuBigEndian();
    if (!isGpuBigEndianResult.has_value()) {
      throw std::runtime_error(isGpuBigEndianResult.error().message());
    }

    auto simulatorResult = GpuSimulator::create({ .width = 5, .height = 5 });
    if (!simulatorResult.has_value()) {
      throw std::runtime_error(simulatorResult.error().message());
    }
    simulator = std::move(*simulatorResult);

    canvasDescription = {
      .size = {
        .width = 5 + 2 * PADDING_SIZE,
        .height = 5 + 2 * PADDING_SIZE
      },
      .paddingSize = PADDING_SIZE,
      .valueOffset = static_cast<uint32_t>(3 * *isGpuBigEndianResult),
      .valueStride = 4
    };

    inputBuffer = std::vector<uint8_t>(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride, 0);
    outputBuffer = std::vector<uint8_t>(canvasDescription.size.width * canvasDescription.size.height * canvasDescription.valueStride, 0);

    auto inputSSBOResult = ShaderStorageBuffer::create(inputBuffer.size());
    if (!inputSSBOResult.has_value()) {
      throw std::runtime_error(inputSSBOResult.error().message());
    }
    inputSSBO = std::move(*inputSSBOResult);

    auto outputSSBOResult = ShaderStorageBuffer::create(outputBuffer.size());
    if (!outputSSBOResult.has_value()) {
      throw std::runtime_error(outputSSBOResult.error().message());
    }
    outputSSBO = std::move(*outputSSBOResult);

    auto textureResult = Texture::create(canvasDescription.size.width, canvasDescription.size.height);
    if (!textureResult.has_value()) {
      throw std::runtime_error(textureResult.error().message());
    }
    texture = std::move(*textureResult);

    resetBuffer(inputBuffer);
    resetBuffer(outputBuffer);
  }

  ~TestFixture() {
    // Overwrite OpenGL objects with nulls so they are freed before deinitOpenGL
    simulator = std::nullopt;
    inputSSBO = std::nullopt;
    outputSSBO = std::nullopt;
    texture = std::nullopt;

    deinitOpenGL();
  }

protected:
  /**
   * @brief Set input value at position
   * 
   * @param position { .x = 0, .y = 0 } means centre of 5x5 grid
   * @param cell 
   */
  auto set(Position2D<int32_t> position, uint8_t cell) -> void {
    const uint32_t idx = mapSimulationPositionToCanvasIndex(
      {
        .x = PADDING_SIZE + 2 + position.x,
        .y = PADDING_SIZE + 2 + position.y
      },
      canvasDescription
    );

    inputBuffer[idx] = cell;
  }

  /**
   * @brief Set expected value at position
   * 
   * @param position { .x = 0, .y = 0 } means centre of 5x5 grid
   * @param cell 
   */
  auto expect(Position2D<int32_t> position, uint8_t cellMask) -> void {
    expectations.push_back({
      .position = position,
      .cellMask = cellMask
    });
  }

  auto runTestExpectAnySuccess(uint32_t tries = 1) -> void {
    std::vector<std::vector<Error>> allErrors;

    for (uint32_t i = 0; i < tries; ++i) {
      std::vector<Error> errors = runTest();
      if (errors.empty()) {
        SUCCEED("All expectations matched");
        return;
      }

      allErrors.emplace_back(std::move(errors));
    }

    for (uint32_t i = 0; i < tries; ++i) {
      INFO("Try " << i);
      checkErrors(allErrors[i]);
    }
  }

  auto runTestExpectAllSuccess(uint32_t tries = 1) -> void {
    for (uint32_t i = 0; i < tries; ++i) {
      INFO("Try " << i);

      std::vector<Error> errors = runTest();
      CHECK(errors.empty());

      checkErrors(errors);
    }
  }

private:
  std::optional<GpuSimulator> simulator;
  std::optional<ShaderStorageBuffer> inputSSBO;
  std::optional<ShaderStorageBuffer> outputSSBO;
  std::optional<Texture> texture;

  PaintingCanvasDescription canvasDescription;

  std::vector<uint8_t> inputBuffer;
  std::vector<uint8_t> outputBuffer;
  std::vector<Expectation> expectations;

  /**
   * @brief Resets input and output to default state
   */
  auto resetBuffer(std::vector<uint8_t>& buffer) -> void {
    Position2D<uint32_t> position;
    for (position.y = 0; position.y < canvasDescription.size.height; ++position.y) {
      for (position.x = 0; position.x < canvasDescription.size.width; ++position.x) {
        const auto idx = mapSimulationPositionToCanvasIndex(position, canvasDescription);
        buffer[idx] = cell::PADDING;
      }
    }
  }

  /**
   * @brief Maps relative position to bufferIndex
   * 
   * @param position 
   * @return uint32_t 
   */
  auto positionToIndex(Position2D<int32_t> position) -> uint32_t {
    const Position2D<uint32_t> absolutePosition = {
      .x = PADDING_SIZE + 2 + position.x,
      .y = PADDING_SIZE + 2 + position.y
    };
    const uint32_t idx = mapSimulationPositionToCanvasIndex(absolutePosition, canvasDescription);

    return idx;
  }

  auto runTest() -> std::vector<Error> {
    std::vector<Error> errors;

    resetBuffer(outputBuffer);
    inputSSBO->store(inputBuffer);
    outputSSBO->store(outputBuffer);

    auto runResult = simulator->run(
      *inputSSBO,
      *outputSSBO,
      *texture
    );
    if (!runResult.has_value()) {
      throw std::runtime_error(runResult.error().message());
    }

    outputSSBO->load(outputBuffer);

    for (const auto& expectation : expectations) {
      const uint32_t idx = positionToIndex(expectation.position);
      if ((outputBuffer[idx] & expectation.cellMask) == 0) {
        errors.push_back({
          .position = expectation.position,
          .cell = outputBuffer[idx],
          .expectedMask = expectation.cellMask
        });
      }
    }

    return errors;
  }

  auto checkErrors(const std::vector<Error>& errors) -> void {
    for (const auto& [position, cell, expectedMask] : errors) {
      INFO("Position { x = " << position.x << ", y = " << position.y << " }");
      INFO("Expected mask " << std::bitset<8>(expectedMask));
      CHECK((cell & expectedMask) > 0);
    }
  }

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

TEST_CASE_METHOD(TestFixture, "AIR should not fall down through SAND") {
  set({ 0, 1 }, cell::AIR);
  set({ 0, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall down through WATER_L") {
  set({ 0, 1 }, cell::AIR);
  set({ 0, 0 }, cell::WATER_L);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_L | cell::WATER_R);
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall down through WATER_R") {
  set({ 0, 1 }, cell::AIR);
  set({ 0, 0 }, cell::WATER_R);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_R | cell::WATER_L);
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should fall down through AIR") {
  set({ 0, 1 }, cell::SAND);
  set({ 0, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "SAND should fall down through WATER_L") {
  set({ 0, 1 }, cell::SAND);
  set({ 0, 0 }, cell::WATER_L);
  expect({ 0, 1 }, cell::WATER_L);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "SAND should fall down through WATER_R") {
  set({ 0, 1 }, cell::SAND);
  set({ 0, 0 }, cell::WATER_R);
  expect({ 0, 1 }, cell::WATER_R);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "WATER_L should fall down through AIR") {
  set({ 0, 1 }, cell::WATER_L);
  set({ 0, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_L);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "WATER_R should fall down through AIR") {
  set({ 0, 1 }, cell::WATER_R);
  set({ 0, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_R);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "WATER_L should not fall down through SAND") {
  set({ 0, 1 }, cell::WATER_L);
  set({ 0, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_L | cell::WATER_R);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should not fall down through SAND") {
  set({ 0, 1 }, cell::WATER_R);
  set({ 0, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_R | cell::WATER_L);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally left through SAND") {
  set({ 0, 1 }, cell::AIR);
  set({ -1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0 }, cell::SAND);

  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally right through SAND") {
  set({ 0, 1 }, cell::AIR);
  set({ 1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0 }, cell::SAND);

  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally left through WATER_L") {
  set({ 0, 1 }, cell::AIR);
  set({ -1, 0, }, cell::WATER_L);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0, }, cell::WATER_L | cell::WATER_R);

  // run twice because moving diagonally happens in one direction at the time
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally left through WATER_R") {
  set({ 0, 1 }, cell::AIR);
  set({ -1, 0, }, cell::WATER_R);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0, }, cell::WATER_R | cell::WATER_L);

  // run twice because moving diagonally happens in one direction at the time
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally right through WATER_L") {
  set({ 0, 1 }, cell::AIR);
  set({ 1, 0, }, cell::WATER_L);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0, }, cell::WATER_L | cell::WATER_R);

  // run test twice because moving diagonally happens in one direction at the time
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally right through WATER_R") {
  set({ 0, 1 }, cell::AIR);
  set({ 1, 0, }, cell::WATER_R);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0, }, cell::WATER_R | cell::WATER_L);

  // run test twice because moving diagonally happens in one direction at the time
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should fall diagonally left through AIR") {
  set({ 0, 1 }, cell::SAND);
  set({ -1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0 }, cell::SAND);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should fall diagonally right through AIR") {
  set({ 0, 1 }, cell::SAND);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0 }, cell::SAND);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not fall diagonally left through WATER_L") {
  set({  0, 1 }, cell::SAND);
  set({ -1, 0 }, cell::WATER_L);
  expect({  0, 1 }, cell::SAND);
  expect({ -1, 0 }, cell::WATER_L | cell::WATER_R);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not fall diagonally left through WATER_R") {
  set({  0, 1 }, cell::SAND);
  set({ -1, 0 }, cell::WATER_R);
  expect({  0, 1 }, cell::SAND);
  expect({ -1, 0 }, cell::WATER_R | cell::WATER_L);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not fall diagonally right through WATER_L") {
  set({ 0, 1 }, cell::SAND);
  set({ 1, 0 }, cell::WATER_L);
  expect({ 0, 1 }, cell::SAND);
  expect({ 1, 0 }, cell::WATER_L | cell::WATER_R);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not fall diagonally right through WATER_R") {
  set({ 0, 1 }, cell::SAND);
  set({ 1, 0 }, cell::WATER_R);
  expect({ 0, 1 }, cell::SAND);
  expect({ 1, 0 }, cell::WATER_R | cell::WATER_L);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should fall diagonally left through AIR") {
  set({ 0, 1 }, cell::WATER_L);
  set({ -1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should fall diagonally right through AIR") {
  set({ 0, 1 }, cell::WATER_L);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should fall diagonally left through AIR") {
  set({ 0, 1 }, cell::WATER_R);
  set({ -1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0 }, cell::WATER_R);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should fall diagonally right through AIR") {
  set({ 0, 1 }, cell::WATER_R);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0 }, cell::WATER_R);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should not fall diagonally left through SAND") {
  set({ 0, 1 }, cell::WATER_L);
  set({ -1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_L | cell::WATER_R);
  expect({ -1, 0 }, cell::SAND);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should not fall diagonally right through SAND") {
  set({ 0, 1 }, cell::WATER_L);
  set({ 1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_L | cell::WATER_R);
  expect({ 1, 0 }, cell::SAND);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should not fall diagonally left through SAND") {
  set({ 0, 1 }, cell::WATER_R);
  set({ -1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_R | cell::WATER_L);
  expect({ -1, 0 }, cell::SAND);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should not fall diagonally right through SAND") {
  set({ 0, 1 }, cell::WATER_R);
  set({ 1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_R | cell::WATER_L);
  expect({ 1, 0 }, cell::SAND);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not move horizontally left through AIR") {
  set({  0, 0 }, cell::SAND);
  set({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::SAND);
  expect({ -1, 0 }, cell::AIR);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not move horizontally right through AIR") {
  set({ 0, 0 }, cell::SAND);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 0 }, cell::SAND);
  expect({ 1, 0 }, cell::AIR);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should move horizontally left through AIR") {
  set({  0, 0 }, cell::WATER_L);
  set({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::AIR);
  expect({ -1, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L when can't move horizontally left should change to WATER_R") {
  set({ 0, 0 }, cell::WATER_L);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_R);
  expect({ 1, 0 }, cell::AIR);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R when can't move horizontally right should change to WATER_L") {
  set({ -1, 0 }, cell::AIR);
  set({  0, 0 }, cell::WATER_R);
  expect({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should move horizontally right through AIR") {
  set({ 0, 0 }, cell::WATER_R);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 0 }, cell::AIR);
  expect({ 1, 0 }, cell::WATER_R);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should not move horizontally left through SAND and should not move right through AIR but should change to WATER_R") {
  set({ -1, 0 }, cell::SAND);
  set({  0, 0 }, cell::WATER_L);
  set({  1, 0 }, cell::AIR);
  expect({ -1, 0 }, cell::SAND);
  expect({  0, 0 }, cell::WATER_R);
  expect({  1, 0 }, cell::AIR);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should not move horizontally right through SAND and should not move left through AIR but should change to WATER_L") {
  set({ -1, 0 }, cell::AIR);
  set({  0, 0 }, cell::WATER_R);
  set({  1, 0 }, cell::SAND);
  expect({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::WATER_L);
  expect({  1, 0 }, cell::SAND);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L and WATER_R race for center position") {
  set({ -1, 0 }, cell::WATER_R);
  set({  0, 0 }, cell::AIR);
  set({  1, 0 }, cell::WATER_L);
  expect({ -1, 0 }, cell::WATER_R);
  expect({  0, 0 }, cell::WATER_L);
  expect({  1, 0 }, cell::AIR);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R and WATER_L race for center position") {
  set({ -1, 0 }, cell::WATER_R);
  set({  0, 0 }, cell::AIR);
  set({  1, 0 }, cell::WATER_L);
  expect({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::WATER_R);
  expect({  1, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should move horizontally through WATER_R") {
  set({ -1, 0 }, cell::WATER_R);
  set({  0, 0 }, cell::WATER_L);
  expect({ -1, 0 }, cell::WATER_L);
  expect({  0, 0 }, cell::WATER_R);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should move horizontally through WATER_L") {
  set({ 0, 0 }, cell::WATER_R);
  set({ 1, 0 }, cell::WATER_L);
  set({ 0, 0 }, cell::WATER_L);
  set({ 1, 0 }, cell::WATER_R);

  runTestExpectAllSuccess(2);
}
