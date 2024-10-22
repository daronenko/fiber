#include "fiber.hpp"
#include "body.hpp"
#include "task.hpp"
#include "scheduler.hpp"
#include "coroutine.hpp"
#include "stack_allocator.hpp"

#include <cstdlib>

namespace fiber {

////////////////////////////////////////////////////////////////////////////////

static thread_local Fiber* current_fiber = nullptr;

////////////////////////////////////////////////////////////////////////////////

Fiber::Fiber(IScheduler& scheduler, ITask* task)
    : scheduler_(scheduler),
      coroutine_(task) {
}

void Fiber::Schedule() {
  scheduler_.Submit(this);
}

void Fiber::Run() noexcept {
  Step();

  if (coroutine_.IsCompleted()) {
    delete this;
    return;
  }

  Schedule();
}

void Fiber::Yield() {
  current_fiber = nullptr;
  coroutine_.Suspend();
}

void Fiber::Step() {
  current_fiber = this;
  coroutine_.Resume();
}

IScheduler& Fiber::Scheduler() {
  return scheduler_;
}

Fiber& Fiber::Self() {
  return *current_fiber;
}

}  // namespace fiber
