add_subdirectory(brush)

target_sources(
  cellular-automaton-application
  PUBLIC
  ApplicationPainting.cpp
  ApplicationPaintingCallback.cpp
  painting.cpp
)
