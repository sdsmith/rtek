#include "core/utility/assert.h"

#include "core/logging/logging.h"
#include "core/types.h"
#include <cassert>

using namespace rk;

[[noreturn]] void rk::debug_break()
{
    // Break into the debugger
#if RK_COMPILER == RK_COMPILER_MSC
    __debugbreak();
#elif RK_OS != RK_OS_WINDOWS
#    if (RK_COMPILER == RK_COMPILER_GCC) || (RK_COMPILER == RK_COMPILER_CLANG)
#        if RK_TARGET_ARCH_X86
    asm("int 3");
#        else
#            error Unsupported architecture
#        endif
#    else
#        error Unsupported compiler
#    endif
#else
#    error Unsupported compiler
#endif
}

void rk::report_assertion_failure(char const* expr, char const* file, s32 line)
{
    assert(expr);
    assert(file);

    LOG_ERROR("{}:{}: Assertion \"{}\" failed\n", file, line, expr);

    // We are going down, flush
    Logger::flush();
}
