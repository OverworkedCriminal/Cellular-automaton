#include "engine/utils/thread_pool/ThreadPool.hpp"
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <latch>
#include <thread>

using namespace engine;

TEST_CASE("Should return error on size=0") {
  auto poolResult = ThreadPool::create(0);
  REQUIRE_FALSE(poolResult.has_value());
}

TEST_CASE("Should fill vector") {
  std::vector<int32_t> vec(4, 0);

  std::latch latch(vec.size());

  auto pool = ThreadPool::create(vec.size());
  if (!pool.has_value()) {
    FAIL("Pool should have been created");
  }

  for (uint32_t i = 0; i < vec.size(); ++i) {
    pool->run([&vec, &latch, i] {
      vec[i] = i + 1;
      latch.count_down();
    });
  }

  latch.wait();

  for (uint32_t i = 0; i < vec.size(); ++i) {
    CHECK(vec[i] == i + 1);
  }
}

TEST_CASE("Should work on multiple threads") {
  auto pool = ThreadPool::create(4);
  if (!pool.has_value()) {
    FAIL("Pool should have been created");
  }

  std::latch latch(pool->getSize());

  const auto start = std::chrono::high_resolution_clock::now();

  for (uint32_t i = 0; i < pool->getSize(); ++i) {
    pool->run([&latch] {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      latch.count_down();
    });
  }

  latch.wait();

  const auto stop = std::chrono::high_resolution_clock::now();
  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  const auto margin = std::chrono::milliseconds(200);

  REQUIRE(duration < std::chrono::milliseconds(4000) - margin);
}