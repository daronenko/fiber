#pragma once

#include "task.hpp"
#include "coroutine.hpp"
#include "scheduler.hpp"
#include "stack.hpp"

namespace fiber {

// Fiber = stackful coroutine + scheduler

class Fiber : private TaskBase {
 public:
  Fiber(IScheduler& scheduler, ITask* task);

  virtual ~Fiber() = default;

  // ~ System calls
  void Schedule();
  void Yield();

  IScheduler& Scheduler();

  static Fiber& Self();

 private:
  // Task
  void Step();
  void Run() noexcept override;

  IScheduler& scheduler_;
  Coroutine coroutine_;
};

}  // namespace fiber
