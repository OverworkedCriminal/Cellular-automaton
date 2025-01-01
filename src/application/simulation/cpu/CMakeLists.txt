target_sources(
  cellular-automaton-application
  PUBLIC
  CpuApplication.cpp
  CpuSimulator.cpp
  utils.cpp
)

# Testing
add_executable(cpu-simulator-test CpuSimulator.test.cpp)
target_link_libraries(cpu-simulator-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(cpu-simulator-test PRIVATE cellular-automaton-application)
catch_discover_tests(cpu-simulator-test TEST_PREFIX "cpu-simulator:")
