add_library(cellular-automaton-application STATIC)

target_include_directories(
  cellular-automaton-application
  PUBLIC
  "${PROJECT_SOURCE_DIR}/src"
)

add_subdirectory(args)
add_subdirectory(drawing)
add_subdirectory(painting)
add_subdirectory(simulation)

# Dependencies
# --- engine ---
target_link_libraries(
  cellular-automaton-application
  cellular-automaton-engine
)
