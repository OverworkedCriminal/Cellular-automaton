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
if(MSVC)
  target_compile_options(cellular-automaton PRIVATE /W4 /WX)
else()
  target_compile_options(cellular-automaton PRIVATE -Wall -Wextra -Wpedantic)
endif()

#
# Header files
#
target_include_directories(cellular-automaton PUBLIC src)

#
# Testing
#
add_subdirectory(lib/Catch2)
include(CTest)
include(Catch)
enable_testing()

#
# Source files
#
add_subdirectory(src)

