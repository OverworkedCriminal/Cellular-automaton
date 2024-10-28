cmake_minimum_required(VERSION 3.30)

#
# Project information
#
project(cellular-automaton VERSION 1.0.0)

#
# C/C++
#
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Output file
#
add_executable(cellular-automaton)

#
# Header files
#
target_include_directories(cellular-automaton PUBLIC src)

#
# Source files
#
add_subdirectory(src)


#
# Libraries
#
# --- GLFW ---
# - window manager
add_subdirectory(lib/glfw-3.4)
target_link_libraries(cellular-automaton glfw)
# --- GLAD ---
# - openGL loader
# GLFW's OpenGL loader does not provide access to glBindImageTexture
# used with compute shaders that's why GLAD is used here
target_include_directories(cellular-automaton PUBLIC lib/glad/include)
target_sources(cellular-automaton PUBLIC lib/glad/src/glad.c)