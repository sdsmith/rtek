#pragma once

#include "src/core/logging.h"
#include "src/core/types.h"

#include <atomic>
#include <cstdlib>

namespace Rtek
{
    extern std::atomic<s32> g_assert_depth;
    static constexpr s32 s_max_assert_depth = 3;
}

#ifdef NDEBUG
#   define RTK_ASSERT(Condition)
#else
#   define RTK_ASSERT(Condition)                                          \
        if (!(Condition)) {                                               \
            ++g_assert_depth;                                             \
            if (g_assert_depth <= s_max_assert_depth) {                   \
                LOG_ERROR("assert failed: %s\n", #Condition);             \
            }                                                             \
            abort();                                                      \
        }
#endif
