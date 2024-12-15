#ifndef APPLICATION_ARGS_ARGS_HPP
#define APPLICATION_ARGS_ARGS_HPP

#include "application/args/Processor.hpp"
#include "engine/error/Error.hpp"
#include <expected>

struct Args {
  /**
   * @brief
   * Parse input arguments
   * 
   * @param argc 
   * @param argv 
   * @return std::expected<std::optional<Args>, engine::Error>
   * Returns parsed Args.
   * If args contain '-h' or '--help' returns std::nullopt.
   */
  static auto parse(
    int argc,
    const char** argv
  ) -> std::expected<std::optional<Args>, engine::Error>;

  Processor processor;
  uint32_t widthSimulation;
  uint32_t heightSimulation;
  uint32_t widthWindow;
  uint32_t heightWindow;
};

#endif