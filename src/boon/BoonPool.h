#pragma once

#include "boon/Boon.h"

#include <span>

namespace hollow {

// The full catalogue of available boons. Kept as a static array so the
// BoonSelectionScene can index into it without any heap allocation.
struct BoonPool {
    static std::span<const Boon> all();
};

} // namespace hollow
