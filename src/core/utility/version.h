#pragma once

#include "core/types.h"

namespace rk
{
struct Version {
    u32 major;
    u32 minor;

    constexpr Version(u32 major, u32 minor) : major(major), minor(minor) {}
    // to_string
};
} // namespace rk
