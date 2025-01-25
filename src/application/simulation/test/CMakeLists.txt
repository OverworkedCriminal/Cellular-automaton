add_executable(cpu-simulator-test Simulator.test.cpp)
target_compile_definitions(cpu-simulator-test PRIVATE CPU_SIMULATOR)
target_compile_definitions(cpu-simulator-test PRIVATE CPU_SIMULATOR_CORE_COUNT=1)
target_link_libraries(cpu-simulator-test PRIVATE cellular-automaton-application)
target_link_libraries(cpu-simulator-test PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(cpu-simulator-test TEST_PREFIX "cpu-simulator:")

add_executable(cpu-simulator-parallel-test Simulator.test.cpp)
target_compile_definitions(cpu-simulator-parallel-test PRIVATE CPU_SIMULATOR)
target_compile_definitions(cpu-simulator-parallel-test PRIVATE CPU_SIMULATOR_CORE_COUNT=4)
target_link_libraries(cpu-simulator-parallel-test PRIVATE cellular-automaton-application)
target_link_libraries(cpu-simulator-parallel-test PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(cpu-simulator-parallel-test TEST_PREFIX "cpu-simulator-parallel:")

add_executable(gpu-simulator-test Simulator.test.cpp)
target_compile_definitions(gpu-simulator-test PRIVATE GPU_SIMULATOR)
target_link_libraries(gpu-simulator-test PRIVATE cellular-automaton-application)
target_link_libraries(gpu-simulator-test PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(
  gpu-simulator-test
  TEST_PREFIX "gpu-simulator:"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)