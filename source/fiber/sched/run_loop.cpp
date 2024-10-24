#include "run_loop.hpp"

#include <fiber/sched/task/task.hpp>

namespace fiber::sched {

void RunLoop::Submit(task::TaskBase* task) {
  tasks_.PushBack(task);
}

// Run tasks

size_t RunLoop::RunAtMost(size_t limit) {
  size_t completed = 0;
  for (; completed < limit && NonEmpty(); ++completed) {
    tasks_.PopFront()->Run();
  }

  return completed;
}

size_t RunLoop::Run() {
  size_t completed = 0;
  for (; NonEmpty(); ++completed) {
    tasks_.PopFront()->Run();
  }

  return completed;
}

}  // namespace fiber::sched
