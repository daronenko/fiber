#pragma once

#include <fiber/sched/task/task.hpp>

#include <wheels/intrusive/forward_list.hpp>

#include <optional>
#include <mutex>
#include <condition_variable>

namespace fiber::sched::intrusive {

// Intrusive unbounded blocking multi-producers/multi-consumers (MPMC) queue

template <typename T>
class IntrusiveUnboundedBlockingQueue {
  using Node = wheels::IntrusiveForwardListNode<T>;

 public:
  bool Push(Node* value) {
    std::lock_guard guard(mutex_);

    if (is_closed_) {
      return false;
    }

    queue_.PushBack(value);
    not_empty_.notify_one();

    return true;
  }

  std::optional<T*> Pop() {
    std::unique_lock lock(mutex_);

    while (queue_.IsEmpty()) {
      if (is_closed_) {
        return std::nullopt;
      }
      not_empty_.wait(lock);
    }

    return queue_.PopFront();
  }

  void Close() {
    std::lock_guard guard(mutex_);
    is_closed_ = true;
    not_empty_.notify_all();
  }

 private:
  bool is_closed_{false};

  std::mutex mutex_;
  std::condition_variable not_empty_;

  wheels::IntrusiveForwardList<T> queue_;
};

}  // namespace fiber::sched::intrusive