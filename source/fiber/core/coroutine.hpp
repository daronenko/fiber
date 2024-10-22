#pragma once

#include "task.hpp"
#include "stack.hpp"

#include <sure/context.hpp>
#include <function2/function2.hpp>

namespace fiber {

class Coroutine : private sure::ITrampoline {
 public:
  explicit Coroutine(ITask*);

  ~Coroutine();

  void Resume();

  void Suspend();

  bool IsCompleted() const;

 private:
  void SetupExecutionContext();

  // sure::ITrampoline
  [[noreturn]] void Run() noexcept override;

 private:
  bool is_completed_{false};
  ITask* task_;
  Stack stack_;
  sure::ExecutionContext coro_ctx_;
  sure::ExecutionContext parent_ctx_;
};

}  // namespace fiber
