#pragma once

#include <stdint.h>

namespace Rtek
{
    using s8  = int8_t;
    using s16 = int16_t;
    using s32 = int32_t;
    using s64 = int64_t;

    using u8  = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using f32 = float;
    using f64 = double;

    constexpr auto operator""_KB(u64 s) {
        return s * 1024;
    }

    constexpr auto operator""_MB(u64 s) {
        return s * 1024_KB;
    }

    constexpr auto operator""_GB(u64 s) {
        return s * 1024_MB;
    }
}
