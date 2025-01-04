target_sources(
  cellular-automaton-application
  PUBLIC
  SquarePaintingBrush.cpp
)

# Testing

add_executable(square-painting-brush-test SquarePaintingBrush.test.cpp)
target_link_libraries(square-painting-brush-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(square-painting-brush-test PRIVATE cellular-automaton-application)
catch_discover_tests(square-painting-brush-test TEST_PREFIX "square-painting-brush:")
