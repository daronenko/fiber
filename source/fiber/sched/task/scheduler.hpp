#pragma once

#include "task.hpp"

namespace fiber::sched::task {

struct IScheduler {
  virtual ~IScheduler() = default;

  virtual void Submit(TaskBase*) = 0;
};

}  // namespace fiber::sched::task
