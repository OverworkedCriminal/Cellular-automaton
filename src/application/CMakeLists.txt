add_library(cellular-automaton-application STATIC)
if(MSVC)
  target_compile_options(cellular-automaton-application PRIVATE /W4 /WX)
else()
  target_compile_options(cellular-automaton-application PRIVATE -Wall -Wextra -Wpedantic)
endif()

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
