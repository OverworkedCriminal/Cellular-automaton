target_sources(
  cellular-automaton-engine
  PUBLIC
  error.cpp
  file.cpp
)

add_subdirectory(dto)
add_subdirectory(thread_pool)
