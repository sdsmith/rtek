#include "core/assert.h"

#include "core/logging/logging.h"
#include "core/types.h"
#include <cassert>

using namespace rk;
using namespace sds;

void rk::debug_break()
{
    // Break into the debugger
#if SDS_COMPILER_MSC
    __debugbreak();
#elif SDS_OS_WINDOWS
#   if SDS_COMPILER_CLANG && __has_builtin(__builtin_debugtrap)
        __builtin_debugtrap();
#   else
#       error Unsupported compiler
#   endif
#else
#    if SDS_COMPILER_GCC || SDS_COMPILER_CLANG
#        if SDS_ARCH_X86 || SDS_ARCH_AMD64
    asm("int 3");
#        else
#            error Unsupported architecture
#        endif
#    else
#        error Unsupported compiler
#    endif
#endif
}

void rk::report_assertion_failure(char const* expr, char const* file, s32 line)
{
    assert(expr);
    assert(file);

    LOG_ERROR("{}:{}: Assertion \"{}\" failed\n", file, line, expr);

    // We are going down, flush logger
    Logger::emergency_shutdown();
}
