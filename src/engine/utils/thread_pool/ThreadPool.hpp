#ifndef ENGINE_UTILS_THREAD_POOL_THREAD_POOL_HPP
#define ENGINE_UTILS_THREAD_POOL_THREAD_POOL_HPP

#include "engine/error/Error.hpp"
#include <condition_variable>
#include <expected>
#include <functional>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

namespace engine {

class ThreadPool {
public:
  static auto create(uint32_t size) -> std::expected<ThreadPool, Error>;

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = default;
  ~ThreadPool();

  auto operator=(const ThreadPool&) -> ThreadPool& = delete;
  auto operator=(ThreadPool&&) -> ThreadPool& = default;

  auto run(std::function<void()> job) -> void;
  auto getSize() const -> uint32_t;

private:
  struct Context {
    std::mutex mutex;
    std::condition_variable cv;
    std::queue<std::function<void()>> jobs;
    bool shouldClose;
  };

  static auto worker_thread_fn(std::shared_ptr<Context> context) -> void;

  ThreadPool(
    std::shared_ptr<Context>&& context,
    std::vector<std::thread>&& threads
  );

  std::shared_ptr<Context> m_context;
  std::vector<std::thread> m_threads;
};

};

#endif