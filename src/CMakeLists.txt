target_sources(cellular-automaton PRIVATE main.cpp)

add_subdirectory(application)
add_subdirectory(engine)

# dependencies
# --- application ---
target_link_libraries(cellular-automaton cellular-automaton-application)
