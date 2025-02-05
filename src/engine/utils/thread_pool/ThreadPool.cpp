#include "engine/utils/thread_pool/ThreadPool.hpp"
#include "engine/utils/error.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace engine {

auto ThreadPool::worker_thread_fn(std::shared_ptr<Context> context) -> void {
  while (true) {
    std::function<void()> job;
    {
      std::unique_lock<std::mutex> lock(context->mutex);
      context->cv.wait(lock, [context] { return !context->jobs.empty() || context->shouldClose; });
      if (context->shouldClose) {
        break;
      }

      job = std::move(context->jobs.front());
      context->jobs.pop();
    }

    job();
  }
}

auto ThreadPool::create(uint32_t size) -> std::expected<ThreadPool, Error> {
  if (size == 0) {
    return std::unexpected(error("ThreadPool size must be positive"));
  }

  auto context = std::make_shared<Context>();
  context->shouldClose = false;

  std::vector<std::thread> threads;
  threads.reserve(size);

  for (uint32_t i = 0; i < size; ++i) {
    threads.emplace_back(ThreadPool::worker_thread_fn, context);
  }

  return ThreadPool(
    std::move(context),
    std::move(threads)
  );
}

ThreadPool::ThreadPool(
  std::shared_ptr<Context>&& context,
  std::vector<std::thread>&& threads
) 
  :m_context(std::move(context))
  ,m_threads(std::move(threads))
{}

ThreadPool::~ThreadPool() {
  // Size of m_threads can only change on std::move.
  // When that happens new object should take care of destruction.
  if (m_threads.empty()) {
    return;
  }

  {
    std::lock_guard<std::mutex> lock(m_context->mutex);
    m_context->shouldClose = true;
  }
  m_context->cv.notify_all();

  for (auto& thread : m_threads) {
    thread.join();
  }
}

auto ThreadPool::run(std::function<void()> job) -> void {
  {
    std::lock_guard<std::mutex> lock(m_context->mutex);
    m_context->jobs.emplace(std::move(job));
  }
  m_context->cv.notify_one();
}

auto ThreadPool::getSize() const -> uint32_t {
  return m_threads.size();
}

}