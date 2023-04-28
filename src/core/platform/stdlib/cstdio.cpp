#include "core/platform/stdlib/cstdio.h"

#include "core/assert.h"
#include <nowide/cstdio.hpp>

using namespace rk;
using namespace sds;

FILE* rk::freopen(char const* file_name, char const* mode, FILE* stream) noexcept
{
    RK_ASSERT(mode);
    RK_ASSERT(stream);
    return nowide::freopen(file_name, mode, stream);
}

FILE* rk::fopen(char const* file_name, char const* mode) noexcept
{
    RK_ASSERT(file_name);
    RK_ASSERT(mode);
    return nowide::fopen(file_name, mode);
}

s32 rk::rename(char const* old_name, char const* new_name) noexcept
{
    RK_ASSERT(old_name);
    RK_ASSERT(new_name);
    return nowide::rename(old_name, new_name);
}

s32 rk::remove(char const* name) noexcept
{
    RK_ASSERT(name);
    return nowide::remove(name);
}
