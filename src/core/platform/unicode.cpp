#include "core/platform/unicode.h"

#include "core/assert.h"

using namespace rk;

bool unicode::ustrcmp(uchar const* s1, uchar const* s2, s32 len) noexcept
{
    RK_ASSERT(s1);
    RK_ASSERT(s2);
    RK_ASSERT(len > 0);

    uchar const* p1 = s1;
    uchar const* p2 = s2;
    s32 n = 0;
    while (n < len) {
        if (*p1 == UC('\0') || *p2 == UC('\0') || *p1 != *p2) { return false; }
        ++n;
        ++p1;
        ++p2;
    }

    return true;
}
