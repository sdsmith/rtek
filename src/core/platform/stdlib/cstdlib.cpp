#include "core/platform/stdlib/cstdlib.h"

#include "core/assert.h"
#include <nowide/cstdlib.hpp>

using namespace rk;
using namespace sds;

char* rk::getenv(const char* key) noexcept
{
    RK_ASSERT(key);
    return nowide::getenv(key);
}

int rk::system(const char* cmd) noexcept
{
    RK_ASSERT(cmd);
    return nowide::system(cmd);
}

bool rk::setenv(const char* key, const char* value, bool overwrite) noexcept
{
    RK_ASSERT(key);
    return nowide::setenv(key, value, overwrite);
}

bool rk::unsetenv(const char* key) noexcept
{
    RK_ASSERT(key);
    return nowide::unsetenv(key);
}

bool rk::putenv(char* string) noexcept
{
    RK_ASSERT(string);
    return nowide::putenv(string);
}
