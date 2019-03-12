#pragma once

#include <cassert>

#define RTK_ASSERT(Condition)                   \
    if (!Condition) {                           \
        LOG_ERROR("%s:%d in function %s: assert failed: %s\n", __FILE__, __LINE__, RTK_FUNC_NAME, #Condition); \
        assert(false);                          \
    }