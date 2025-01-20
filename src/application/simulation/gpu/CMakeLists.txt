target_sources(
  cellular-automaton-application
  PUBLIC
  GpuApplication.cpp
  GpuSimulator.cpp
  utils.cpp
)

add_executable(gpu-simulator-test GpuSimulator.test.cpp)
target_link_libraries(gpu-simulator-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(gpu-simulator-test PRIVATE cellular-automaton-application)
catch_discover_tests(
  gpu-simulator-test
  TEST_PREFIX "gpu-simulator:"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
