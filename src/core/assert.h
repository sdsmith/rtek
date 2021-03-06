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

/**
 * \def RK_STATIC_ASSERT(expr)
 * \brief Static assertion.
 *
 * \param expr Expression.
 */

/**
 * \def RK_STATIC_ASSERT_MSG(expr, msg)
 * \brief Static assertion with a custom message on failure.
 *
 * \param expr Expression.
 * \param msg Error message.
 */
#define RK_I_ASSERT_GLUE(a, b) a##b
#define RK_ASSERT_GLUE(a, b) RK_I_ASSERT_GLUE(a, b)

#ifdef RK_CPLUSPLUS
#    if RK_CPLUSPLUS >= RK_CPLUSPLUS_11
#        define RK_STATIC_ASSERT(expr) static_assert(RK_LIKELY(expr), "static assert failed:" #        expr)
#        define RK_STATIC_ASSERT_MSG(expr, msg) static_assert(RK_LIKELY(expr), msg);
#    else
// no static_assert prior to c++11
template <bool>
class Static_Assert;
template <>
class Static_Assert<true> {};
#        define RK_STATIC_ASSERT(expr) \
            enum { RK_ASSERT_GLUE(g_assert_fail_, __LINE__) = sizeof(Static_Assert<!!(expr)>) }
#        define RK_STATIC_ASSERT_MSG(expr, msg) RK_STATIC_ASSERT(expr)
#    endif
#endif

#undef RK_ASSERT_GLUE
#undef RK_I_ASSERT_GLUE

} // namespace rk
