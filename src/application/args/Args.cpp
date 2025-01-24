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

enum class State : uint8_t {
  PARSING_OPTION,
  PARSING_VALUE,
  PARSING_OPTIONAL_VALUE
};

struct Context {
  /**
   * Parser state
   */
  State state;

  /**
   * Arguments parsed by the parser
   */
  Args* args;

  /**
   * Pointer to value for parsing value
   */
  uint32_t* valuePtr;
};

static auto parseUint32(const char* str) -> expected<uint32_t, Error> {
  uint32_t value = 0;
  uint32_t i = 0;

  while (str[i] != '\0') {
    if (str[i] < '0' || str[i] > '9') {
      return unexpected(error("invalid uint32_t character"));
    }

    value = value * 10 + (str[i] - '0');
    i += 1;
  }

  return value;
}

static auto printHelp() -> void {
  std::cout << "cellular-automaton -w <width> -h <height>\n"
            << "\nARGS:\n"
            << '\t' << std::left << std::setw(17) << "--cpu [coreCount]" << " run simulation on CPU (default). Default coreCount is 1\n"
            << '\t' << std::left << std::setw(17) << "--gpu" << " run simulation on GPU\n"
            << '\t' << std::left << std::setw(17) << "-w, --width" << " simulation width\n"
            << '\t' << std::left << std::setw(17) << "-h, --height" << " simulation height\n"
            << '\t' << std::left << std::setw(17) << "--window-width" << " simulation width\n"
            << '\t' << std::left << std::setw(17) << "--window-height" << " simulation height\n"
            << '\t' << std::left << std::setw(17) << "--help" << " display this message\n";
}

static auto parseOption(Context& context, const char* option) -> expected<void, Error> {
  if (std::strcmp(option, "--cpu") == 0) {
    context.args->processor = Processor::CPU;
    context.state = State::PARSING_OPTIONAL_VALUE;
    context.valuePtr = &context.args->processorCoreCount;
    return {};
  }
  if (std::strcmp(option, "--gpu") == 0) {
    context.args->processor = Processor::GPU;
    return {};
  }

  if (std::strcmp(option, "-w") == 0 || std::strcmp(option, "--width") == 0) {
    context.state = State::PARSING_VALUE;
    context.valuePtr = &context.args->widthSimulation;
    return {};
  }
  if (std::strcmp(option, "-h") == 0 || std::strcmp(option, "--height") == 0) {
    context.state = State::PARSING_VALUE;
    context.valuePtr = &context.args->heightSimulation;
    return {};
  }

  if (std::strcmp(option, "--window-width") == 0) {
    context.state = State::PARSING_VALUE;
    context.valuePtr = &context.args->widthWindow;
    return {};
  }
  if (std::strcmp(option, "--window-height") == 0) {
    context.state = State::PARSING_VALUE;
    context.valuePtr = &context.args->heightWindow;
    return {};
  }

  return unexpected(error("unexpected option"));
}

static auto parseValue(Context& context, const char* value) -> expected<void, Error> {
  auto result = parseUint32(value);
  if (!result.has_value()) {
    return unexpected(error("failed to parse value", result.error()));
  }

  context.state = State::PARSING_OPTION;
  *context.valuePtr = *result;
  context.valuePtr = nullptr;

  return {};
}

auto Args::parse(
  int argc,
  const char **argv
) -> expected<optional<Args>, Error> {
  Args args = {
    .processor = Processor::CPU,
    .processorCoreCount = 1,
    .widthSimulation = 0,
    .heightSimulation = 0,
    .widthWindow = 800,
    .heightWindow = 600
  };

  Context context = {
    .state = State::PARSING_OPTION,
    .args = &args,
    .valuePtr = nullptr
  };

  for (uint32_t i = 1; i < argc; ++i) {
    expected<void, Error> result;

    switch (context.state) {
      case State::PARSING_OPTION: {
        if (std::strcmp(argv[i], "--help") == 0) {
          printHelp();
          return std::nullopt;
        }
        result = parseOption(context, argv[i]);
        break;
      }
      case State::PARSING_VALUE: {
        result = parseValue(context, argv[i]);
        break;
      }
      case State::PARSING_OPTIONAL_VALUE: {
        result = parseValue(context, argv[i]);
        if (!result.has_value()) {
          context.state = State::PARSING_OPTION;
          context.valuePtr = nullptr;
        }
        result = {};
        break;
      }
    }

    if (!result.has_value()) {
      return unexpected(error("parsing input argument failed", result.error()));
    }
  }

  if (args.widthSimulation == 0) {
    return unexpected(error("width is required and must be positive"));
  }
  if (args.heightSimulation == 0) {
    return unexpected(error("height is required and must be positive"));
  }

  return args;
}