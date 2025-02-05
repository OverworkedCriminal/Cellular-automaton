#include "application/args/Args.hpp"
#include "application/simulation/cpu/CpuApplication.hpp"
#include "application/simulation/gpu/GpuApplication.hpp"
#include "engine/Config.hpp"
#include "engine/application/IApplication.hpp"
#include "engine/engine.hpp"
#include "engine/utils/dto/Size2D.hpp"
#include <iostream>
#include <memory>

using engine::Config;
using engine::Size2D;
using engine::IApplication;

int main(int argc, const char** argv) {
  const auto argsResult = Args::parse(argc, argv);
  if (!argsResult.has_value()) {
    std::cerr << "Failed to parse input arguments:\n\t" << argsResult.error() << '\n';
    return -1;
  }
  const auto& argsOpt = *argsResult;
  if (!argsOpt.has_value()) {
    // program was called with '--help' flag
    return 0;
  }
  const auto& args = *argsOpt;

  const Config config = {
    .windowTitle = "Cellular automaton",
    .windowWidth = static_cast<int>(args.widthWindow),
    .windowHeight = static_cast<int>(args.heightWindow)
  };

  const Size2D<uint32_t> simulationSize = {
    .width = args.widthSimulation,
    .height = args.heightSimulation
  };

  std::unique_ptr<IApplication> applicationPtr;
  switch (args.processor) {
    case Processor::CPU: {
      auto applicationResult = CpuApplication::create(
        simulationSize,
        args.processorCoreCount
      );
      if (!applicationResult.has_value()) {
        std::cerr << "CpuApplication creation failed:\n\t" << applicationResult.error() << '\n';
        return -1;
      }
      applicationPtr = std::make_unique<CpuApplication>(std::move(*applicationResult));
      break;
    }
    case Processor::GPU: {
      auto applicationResult = GpuApplication::create(simulationSize);
      if (!applicationResult.has_value()) {
        std::cerr << "GpuApplication creation failed:\n\t" << applicationResult.error() << '\n';
        return -1;
      }
      applicationPtr = std::make_unique<GpuApplication>(std::move(*applicationResult));
      break;
    }
  }

  auto result = run(config, std::move(applicationPtr));
  if (!result.has_value()) {
    std::cerr << "Engine failed:\n\t" << result.error() << '\n';
    return -1;
  }

  return 0;
}