#pragma once

#include "scheduler.hpp"
#include "unique_function.hpp"

#include <cstdlib>

namespace fiber::sched::task {

template <typename F>
void Submit(IScheduler& scheduler, F func) {
  auto task = UniqueFunction<F>::New(std::move(func));
  scheduler.Submit(task);
}

}  // namespace fiber::sched::task
