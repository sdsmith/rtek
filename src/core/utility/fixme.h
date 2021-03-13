#pragma once

#include "core/assert.h"
#include "core/logging/logging.h"

/**
 * \file fixme.h
 * \brief Helper to mark things that should be fixed but are long term goals and need temporary
 * bandaids.
 */

/**
 * \brief Log a "fix me". Toggled with \a RK_LOG_FIX_ME.
 */
#ifdef RK_LOG_FIXME
#    define LOG_FIXME(...) \
        ::rk::Logger::log(spdlog::level::warn, RK_FILENAME, __LINE__, RK_FUNCNAME, __VA_ARGS__)
#else
#    define LOG_FIXME(...) static_cast<void>(0)
#endif

namespace fixme
{
/**
 * \brief Wrapper for a `static_cast` that should be fixed.
 *
 * \param reason Reason that the cast should be fixed.
 */
template <typename U, typename V>
U scast(V a, char const* reason)
{
    RK_ASSERT(reason);
    LOG_FIXME(reason);

    return static_cast<U>(a);
}
}; // namespace fixme
