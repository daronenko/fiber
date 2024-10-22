#pragma once

#include "stack.hpp"

#include <optional>
#include <vector>

namespace fiber::core {

Stack AllocateStack();

void ReleaseStack(Stack);

}  // namespace fiber::core
