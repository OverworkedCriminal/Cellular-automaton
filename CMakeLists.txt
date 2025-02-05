cmake_minimum_required(VERSION 3.30)

project(cellular-automaton VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


#
# Output file
#
add_executable(cellular-automaton)
if(MSVC)
  target_compile_options(cellular-automaton PRIVATE /W4 /WX)
else()
  target_compile_options(cellular-automaton PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_include_directories(cellular-automaton PUBLIC src)


#
# Dependencies
#
include(FetchContent)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.0
)
FetchContent_MakeAvailable(Catch2)


#
# Testing
#
include(CTest)
include(Catch)
enable_testing()


#
# Source files
#
add_subdirectory(src)
