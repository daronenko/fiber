#include "stack_allocator.hpp"
#include "stack.hpp"

namespace fiber::core {

////////////////////////////////////////////////////////////////////////////////

static const size_t kDefaultStackSize = 64 * 1024;  // 64 KiB

////////////////////////////////////////////////////////////////////////////////

class StackAllocator {
 public:
  Stack Allocate() {
    if (auto stack = TryTakeFromPool()) {
      return std::move(*stack);
    }
    return AllocateNew();
  }

  void Release(Stack stack) {
    pool_.push_back(std::move(stack));
  }

 private:
  static Stack AllocateNew() {
    return Stack::AllocateBytes(/*at_least=*/kDefaultStackSize);
  }

  std::optional<Stack> TryTakeFromPool() {
    if (pool_.empty()) {
      return std::nullopt;
    }

    Stack stack = std::move(pool_.back());
    pool_.pop_back();
    return stack;
  }

 private:
  std::vector<Stack> pool_;
};

//////////////////////////////////////////////////////////////////////

// StackAllocator allocator;

// Stack AllocateStack() {
//   return allocator.Allocate();
// }

// void ReleaseStack(Stack stack) {
//   allocator.Release(std::move(stack));
// }

Stack AllocateStack() {
  return Stack::AllocateBytes(/*at_least=*/kDefaultStackSize);
}

void ReleaseStack(Stack) {
}

}  // namespace fiber::core
