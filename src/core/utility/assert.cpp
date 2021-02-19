#include "assert.h"

#include "core/logging/logging.h"
#include "core/types.h"

using namespace rk;

[[noreturn]] void rk::debug_break() {
    // Break into the debugger
#if RK_COMPILER == RK_COMPILER_MSC
    __debugbreak();
#elif RK_OS != RK_OS_WINDOWS
#   if (RK_COMPILER == RK_COMPILER_GCC) || (RK_COMPILER == RK_COMPILER_CLANG)
#       if TARGET_ARCH_X86
    asm("int 3");
#       else
#           error Unsupported architecture
#       endif
#   else
#       error Unsupported compiler
#   endif
#else
#   error Unsupported compiler
#endif
}

void rk::report_assertion_failure(char const* expr, char const* file, s32 line) {
    LOG_ERROR("%s:%d: Assertion \"%s\" failed\n", file, line, expr);
}
