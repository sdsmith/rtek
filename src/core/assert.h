#pragma once

#include <cstdlib>

#ifdef NDEBUG
#   define RTK_ASSERT(Condition)
#else
#   define RTK_ASSERT(Condition)                   \
        if (!(Condition)) {                        \
            LOG_ERROR("assert failed: %s\n", #Condition); \
            abort();                               \
        }
#endif