#pragma once

#include <fiber/sched/task/task.hpp>
#include <fiber/sched/task/scheduler.hpp>

#include <wheels/intrusive/forward_list.hpp>

#include <cstdint>

namespace exe::sched {

// Single-threaded task queue

class RunLoop : public task::IScheduler {
 public:
  RunLoop() = default;

  // Non-copyable
  RunLoop(const RunLoop&) = delete;
  RunLoop& operator=(const RunLoop&) = delete;

  // Non-movable
  RunLoop(RunLoop&&) = delete;
  RunLoop& operator=(RunLoop&&) = delete;

  // task::IScheduler
  void Submit(task::TaskBase*) override;

  // Run tasks

  // Run at most `limit` tasks from queue
  // Returns number of completed tasks
  size_t RunAtMost(size_t limit);

  // Run next task if queue is not empty
  bool RunNext() {
    return RunAtMost(1) == 1;
  }

  // Run tasks until queue is empty
  // Returns number of completed tasks
  size_t Run();

  bool IsEmpty() const {
    return tasks_.IsEmpty();
  }

  bool NonEmpty() const {
    return !IsEmpty();
  }

 private:
  wheels::IntrusiveForwardList<task::TaskBase> tasks_;
};

}  // namespace exe::sched
