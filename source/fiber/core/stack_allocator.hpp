#pragma once

#include "stack.hpp"

#include <optional>
#include <vector>

namespace fiber {

Stack AllocateStack();

void ReleaseStack(Stack);

}  // namespace fiber
