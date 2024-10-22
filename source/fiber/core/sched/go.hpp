#pragma once

#include <fiber/core/fiber.hpp>
#include <fiber/core/scheduler.hpp>
#include <fiber/sched/task/unique_function.hpp>

namespace fiber {

// Considered harmful

template <typename F>
void Go(IScheduler& scheduler, F func) {
  auto task = sched::task::UniqueFunction<F>::New(std::move(func));
  (new Fiber(scheduler, std::move(task)))->Schedule();
}

template <typename F>
void Go(F func) {
  Go(Fiber::Self().Scheduler(), std::move(func));
}

}  // namespace fiber
