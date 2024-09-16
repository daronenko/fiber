#include "wait.hpp"

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace fiber::futex {

namespace {

long futex(uint32_t* uaddr, int futex_op, uint32_t val,
           const struct timespec* timeout, uint32_t* uaddr2, uint32_t val3) {
  return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

uint32_t* ExtractPointer(std::atomic<uint32_t>& atomic) {
  return reinterpret_cast<uint32_t*>(&atomic);
}

}  // namespace

void Wait(std::atomic<uint32_t>& atomic, uint32_t old_value,
          std::memory_order mo) {
  while (atomic.load(mo) == old_value) {
    futex(ExtractPointer(atomic), FUTEX_WAIT_PRIVATE, old_value, nullptr,
          nullptr, 0);
  }
}

WakeKey PrepareWake(std::atomic<uint32_t>& atomic) {
  return {ExtractPointer(atomic)};
}

void WakeOne(WakeKey key) {
  futex(key.addr, FUTEX_WAKE_PRIVATE, 1, nullptr, nullptr, 0);
}

void WakeAll(WakeKey key) {
  futex(key.addr, FUTEX_WAKE_PRIVATE, INT32_MAX, nullptr, nullptr, 0);
}

}  // namespace fiber::futex
