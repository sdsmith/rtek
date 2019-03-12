#pragma once

#include "src/core/types.h"

namespace Rtek
{
    struct Version 
    {
        u32 major;
        u32 minor;

        constexpr Version(u32 major, u32 minor) : major(major), minor(minor) {}
        // to_string
    };
}
