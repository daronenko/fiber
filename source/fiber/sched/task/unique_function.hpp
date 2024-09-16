#pragma once

#include "task.hpp"

#include <memory>
#include <utility>

namespace fiber::sched::task {

template <typename F>
class UniqueFunction : TaskBase {
 public:
  static TaskBase* New(F&& func) {
    return new UniqueFunction(std::move(func));
  }

  // Movable

  UniqueFunction(UniqueFunction&& other) noexcept
      : func_(std::move(other.func_)) {
  }

  UniqueFunction& operator=(UniqueFunction&& other) noexcept {
    if (this != &other) {
      func_ = std::move(other.func_);
    }

    return *this;
  }

  // Non-copyable
  UniqueFunction(const UniqueFunction&&) = delete;
  UniqueFunction& operator=(const UniqueFunction&&) = delete;

  void Run() noexcept override {
    func_();
    delete this;
  }

  virtual ~UniqueFunction() = default;

 private:
  explicit UniqueFunction(F&& func)
      : func_(std::move(func)) {
  }

 private:
  F func_;
};

}  // namespace fiber::sched::task
