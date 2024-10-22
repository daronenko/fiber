#pragma once

#include <fiber/core/fiber.hpp>

namespace fiber::core {

void Yield() {
  Fiber::Self().Yield();
}

}  // namespace fiber::core
