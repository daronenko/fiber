#pragma once

#include <fiber/sched/intrusive/blocking_queue.hpp>

#include <fiber/sched/task/task.hpp>
#include <fiber/sched/task/scheduler.hpp>
#include <fiber/sync/wait_group.hpp>

#include <thread>
#include <vector>
#include <cstdint>

namespace fiber::sched {

// Fixed-size pool of worker threads

class ThreadPool : public task::IScheduler {
 public:
  explicit ThreadPool(size_t threads);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // Non-movable
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  void Start();

  // task::IScheduler
  void Submit(task::TaskBase*) override;

  static ThreadPool* Current();

  void Stop();

 private:
  void WorkerRoutine();
  void JoinWorkers();

 private:
  IntrusiveUnboundedBlockingQueue<task::TaskBase> tasks_;

  size_t workers_count_;
  std::vector<std::thread> workers_;
};

}  // namespace fiber::sched
