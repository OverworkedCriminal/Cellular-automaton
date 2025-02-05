target_sources(
  cellular-automaton-application
  PUBLIC
  position_mapping.cpp
)

add_executable(position-mapping-test position_mapping.test.cpp)
target_link_libraries(position-mapping-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(position-mapping-test PRIVATE cellular-automaton-application)
catch_discover_tests(position-mapping-test TEST_PREFIX "position-mapping-test:")