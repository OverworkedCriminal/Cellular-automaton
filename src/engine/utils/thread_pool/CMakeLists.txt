target_sources(cellular-automaton-engine PUBLIC ThreadPool.cpp)

add_executable(thread-pool-test ThreadPool.test.cpp)
target_link_libraries(thread-pool-test PRIVATE cellular-automaton-engine)
target_link_libraries(thread-pool-test PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(thread-pool-test TEST_PREFIX "thread-pool:")