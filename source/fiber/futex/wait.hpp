#pragma once

#include <atomic>
#include <cstdint>

namespace fiber::futex {

struct WakeKey {
  uint32_t* addr;
};

void Wait(std::atomic<uint32_t>& atomic, uint32_t old_value,
          std::memory_order mo = std::memory_order_seq_cst);

WakeKey PrepareWake(std::atomic<uint32_t>& atomic);

void WakeOne(WakeKey key);
void WakeAll(WakeKey key);

}  // namespace fiber::futex
