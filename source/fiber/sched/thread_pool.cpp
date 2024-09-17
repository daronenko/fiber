#include "thread_pool.hpp"

#include <wheels/core/panic.hpp>
#include <wheels/core/exception.hpp>

#include <cassert>

namespace fiber::sched {

////////////////////////////////////////////////////////////////////////////////

static thread_local ThreadPool* pool;

////////////////////////////////////////////////////////////////////////////////

ThreadPool::ThreadPool(size_t workers)
    : workers_count_(workers) {
}

void ThreadPool::Start() {
  for (size_t i = 0; i < workers_count_; ++i) {
    workers_.emplace_back([this] {
      WorkerRoutine();
    });
  }
}

void ThreadPool::WorkerRoutine() {
  pool = this;

  while (true) {
    auto task = tasks_.Pop();
    if (!task.has_value()) {
      return;
    }
    try {
      task.value()->Run();
    } catch (...) {
      WHEELS_PANIC("Uncaught exception: " << wheels::CurrentExceptionMessage());
    }
  }
}

ThreadPool::~ThreadPool() {
  assert(workers_.empty());
}

void ThreadPool::Submit(task::TaskBase* task) {
  tasks_.Push(task);
}

ThreadPool* ThreadPool::Current() {
  return pool;
}

void ThreadPool::Stop() {
  tasks_.Close();
  JoinWorkers();
}

void ThreadPool::JoinWorkers() {
  for (auto& worker : workers_) {
    worker.join();
  }
  workers_.clear();
}

}  // namespace fiber::sched
