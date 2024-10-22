#include "yield.hpp"

#include <fiber/core/fiber.hpp>

namespace fiber {

void Yield() {
  Fiber::Self().Yield();
}

}  // namespace fiber
