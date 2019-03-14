#pragma once

#include "src/core/types.h"

#include <atomic>
#include <cstdlib>

namespace Rtek
{
    extern std::atomic<s32> g_assert_depth;
}

#ifdef NDEBUG
#   define RTK_ASSERT(Condition)
#else
#   define RTK_ASSERT(Condition)                   \
        if (!(Condition)) {                        \
            LOG_ERROR("assert failed: %s\n", #Condition); \
            abort();                               \
        }
#endif
