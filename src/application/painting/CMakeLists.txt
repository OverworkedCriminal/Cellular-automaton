add_subdirectory(brush)
add_subdirectory(utils)

target_sources(
  cellular-automaton-application
  PUBLIC
  ApplicationPainting.cpp
  ApplicationPaintingCallback.cpp
)
