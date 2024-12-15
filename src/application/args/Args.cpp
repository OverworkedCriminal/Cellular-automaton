#include "application/args/Args.hpp"
#include "application/args/Processor.hpp"
#include "engine/error/Error.hpp"
#include "engine/utils/error.hpp"
#include <cstdint>
#include <cstring>
#include <expected>
#include <iomanip>
#include <iostream>
#include <optional>

using engine::Error;
using engine::error;
using std::optional;
using std::string;
using std::expected;
using std::unexpected;

static auto parseIntValue(
  int argc,
  const char** argv,
  uint32_t i
) -> expected<optional<int>, Error> {
  if (i >= argc) {
    return unexpected(error("missing value"));
  }

  int value = std::atoi(argv[i]);
  if (value == 0) {
    return unexpected(error("invalid value"));
  }

  return value;
}

auto Args::parse(
  int argc,
  const char **argv
) -> expected<optional<Args>, Error> {
  Processor processor = Processor::CPU;
  optional<uint32_t> widthSimulation;
  optional<uint32_t> heightSimulation;

  for (uint32_t i = 1; i < argc; ++i) {
    auto arg = argv[i];

    if (std::strcmp(argv[i], "--help") == 0) {
      std::cout << "cellular-automaton [cpu|gpu] -w <width> -h <height>\n"
                << "\nARGS:\n"
                << '\t' << std::left << std::setw(12) << "cpu" << " run simulation on CPU (default)\n"
                << '\t' << std::left << std::setw(12) << "gpu" << " run simulation on GPU\n"
                << '\t' << std::left << std::setw(12) << "-w, --width" << " simulation width\n"
                << '\t' << std::left << std::setw(12) << "-h, --height" << " simulation height\n"
                << '\t' << std::left << std::setw(12) << "--help" << " display this message\n";
      return std::nullopt;
    }

    if (std::strcmp(argv[i], "cpu") == 0) {
      processor = Processor::CPU;
      continue;
    }
    if (std::strcmp(argv[i], "gpu") == 0) {
      processor = Processor::GPU;
      continue;
    }

    if (std::strcmp(argv[i], "-w") == 0 || std::strcmp(argv[i], "--width") == 0) {
      auto width = parseIntValue(argc, argv, i + 1);
      if (!width.has_value()) {
        return unexpected(error("failed to parse width", width.error()));
      }
      widthSimulation = *width;
      i += 1; // skip next arg since it already has been parsed
      continue;
    }
    if (std::strcmp(argv[i], "-h") == 0 || std::strcmp(argv[i], "--height") == 0) {
      auto height = parseIntValue(argc, argv, i + 1);
      if (!height.has_value()) {
        return unexpected(error("failed to parse height", height.error()));
      }
      heightSimulation = *height;
      i += 1; // skip next arg since it already has been parsed
      continue;
    }

    return unexpected(error("unexpected option"));
  }

  if (!widthSimulation.has_value()) {
    return unexpected(error("width is required value"));
  }
  if (!heightSimulation.has_value()) {
    return unexpected(error("height is required"));
  }

  return Args {
    .processor = processor,
    .widthSimulation = *widthSimulation,
    .heightSimulation = *heightSimulation
  };
}