add_subdirectory(brush)

target_sources(
  cellular-automaton-application
  PUBLIC
  ApplicationPainting.cpp
  ApplicationPaintingCallback.cpp
  painting.cpp
)

# Testing
add_executable(painting-test painting.test.cpp)
target_link_libraries(painting-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(painting-test PRIVATE cellular-automaton-application)
catch_discover_tests(painting-test TEST_PREFIX "painting:")

add_executable(painting-brush-test PaintingBrush.test.cpp)
target_link_libraries(painting-brush-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(painting-brush-test PRIVATE cellular-automaton-application)
catch_discover_tests(painting-brush-test TEST_PREFIX "painting-brush:")
