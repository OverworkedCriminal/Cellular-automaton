add_library(cellular-automaton-engine STATIC)
if(MSVC)
  target_compile_options(cellular-automaton-engine PRIVATE /W4 /WX)
else()
  target_compile_options(cellular-automaton-engine PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_include_directories(
  cellular-automaton-engine
  PUBLIC
  "${PROJECT_SOURCE_DIR}/src"
)

target_sources(cellular-automaton-engine PUBLIC engine.cpp)

add_subdirectory(application)
add_subdirectory(callback)
add_subdirectory(error)
add_subdirectory(graphics)
add_subdirectory(input)
add_subdirectory(time)
add_subdirectory(utils)
add_subdirectory(window)

# dependencies
# --- GLFW --- # window manager
add_subdirectory("${PROJECT_SOURCE_DIR}/lib/glfw-3.4" lib/glfw-3.4)
target_link_libraries(cellular-automaton-engine glfw)
# --- GLAD --- # openGL loader
# GLFW's OpenGL loader does not provide access to glBindImageTexture
# used with compute shaders that's why GLAD is used here
target_include_directories(cellular-automaton-engine PUBLIC "${PROJECT_SOURCE_DIR}/lib/glad/include")
target_sources(cellular-automaton-engine PUBLIC "${PROJECT_SOURCE_DIR}/lib/glad/src/glad.c")