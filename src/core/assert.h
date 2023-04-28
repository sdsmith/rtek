#pragma once

#include "core/types.h"

namespace rk
{

/**
 * \brief Break the program to the debugger, if available.
 *
 * Architecture specific. Controlled by the \a TARGET_ARCH_* macros.
 */
[[noreturn]] void debug_break();

void report_assertion_failure(char const* expr, char const* file, s32 line);

/**
 * \def RK_CRITICAL_ASSERT(expr)
 * \brief An assert that is present in builds without \a RK_ASSERTIONS_ENABLED set.
 *
 * Meant for assertions that should be run in production.
 */
#define RK_CRITICAL_ASSERT(expr)                                   \
    if RK_LIKELY (expr) {                                          \
    } else {                                                       \
        ::rk::report_assertion_failure(#expr, __FILE__, __LINE__); \
        ::rk::debug_break();                                       \
    }

/**
 * \def RK_ASSERT(expr)
 * \brief Assertion.
 *
 * Only compiled in when \a RK_ASSERTIONS_ENABLED is set.
 *
 * \param expr Expression.
 */
#if RK_ASSERTIONS_ENABLED
#    define RK_ASSERT(expr) RK_CRITICAL_ASSERT(expr)
#else
#    define RK_ASSERT(expr)
#endif

} // namespace rk
