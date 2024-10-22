#include "coroutine.hpp"
#include "stack_allocator.hpp"
#include "body.hpp"
#include "task.hpp"

#include <wheels/core/panic.hpp>
#include <wheels/core/exception.hpp>

namespace fiber::core {

Coroutine::Coroutine(ITask* task)
    : task_(task),
      stack_(AllocateStack()) {
  SetupExecutionContext();
}

Coroutine::~Coroutine() {
  ReleaseStack(std::move(stack_));
}

void Coroutine::SetupExecutionContext() {
  coro_ctx_.Setup(
      /*stack=*/stack_.MutView(),
      /*trampoline=*/this);
}

void Coroutine::Run() noexcept {
  task_->Run();
  is_completed_ = true;
  coro_ctx_.ExitTo(parent_ctx_);
}

void Coroutine::Resume() {
  parent_ctx_.SwitchTo(coro_ctx_);
}

void Coroutine::Suspend() {
  coro_ctx_.SwitchTo(parent_ctx_);
}

bool Coroutine::IsCompleted() const {
  return is_completed_;
}

}  // namespace fiber::core
