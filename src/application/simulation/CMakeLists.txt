add_subdirectory(cpu)
add_subdirectory(gpu)
add_subdirectory(input)

target_sources(cellular-automaton PUBLIC Cell.cpp)