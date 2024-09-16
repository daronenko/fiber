#pragma once

#include <fiber/futex/wait.hpp>

#include <atomic>
#include <cstdint>

namespace fiber::sync {

class WaitGroup {
 public:
  void Add(uint32_t count = 1) {
    counter_.fetch_add(count);
  }

  void Done(uint32_t count = 1) {
    auto key = futex::PrepareWake(counter_);
    if (counter_.fetch_sub(count) - count == 0) {
      futex::WakeAll(key);
    }
  }

  void Wait() {
    auto count = counter_.load();
    while (count != 0) {
      futex::Wait(counter_, count);
      count = counter_.load();
    }
  }

 private:
  std::atomic<uint32_t> counter_{0};
};

}  // namespace fiber::sync
