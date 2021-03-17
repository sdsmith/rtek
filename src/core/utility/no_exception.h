#pragma once

#include "core/logging/logging.h"
#include "core/status.h"
#include <exception>
#include <optional>
#include <type_traits>
#include <variant>

namespace rk
{
#define RK_EXCEPTION_BOUNDARY_CATCH                     \
    catch (const std::logic_error& e)                   \
    {                                                   \
        LOG_ERROR(e.what());                            \
        return Status::logic_error;                     \
    }                                                   \
    catch (const std::runtime_error& e)                 \
    {                                                   \
        LOG_ERROR(e.what());                            \
        return Status::runtime_error;                   \
    }                                                   \
    catch (const std::bad_alloc& e)                     \
    {                                                   \
        LOG_ERROR(e.what());                            \
        return Status::bad_alloc;                       \
    }                                                   \
    catch (const std::bad_exception& e)                 \
    {                                                   \
        LOG_ERROR(e.what());                            \
        return Status::exception_error;                 \
    }                                                   \
    catch (const std::exception& e)                     \
    {                                                   \
        LOG_ERROR("Unhandled exception: {}", e.what()); \
        return Status::generic_error;                   \
    }                                                   \
    catch (...)                                         \
    {                                                   \
        LOG_ERROR("Unknown exception occured");         \
        return Status::generic_error;                   \
    }

/**
 * \brief Stops all exceptions and converts them to status codes.
 *
 * Usage:
 * \code{.cpp}
 * Status foo(int a, int b, bool* out) {
 *     auto r = exception_boundary([&]() {
 *         return a == b;
 *     });
 *
 *     if (auto p_val = std::get_if<0>(&r)) {
 *         *out = *p_val;
 *         return Status::OK;
 *     } else {
 *         // No value
 *         auto p_status = std::get_if<1>(&r);
 *         assert(p_status);
 *         return *p_status;
 *     }
 * }
 * \endcode
 */
template <typename F, typename R = std::invoke_result<F>,
          typename = std::enable_if_t<!std::is_same_v<R, Status>>>
auto exception_boundary(F&& f) noexcept -> std::variant<R, Status>
{
    // TODO(sdsmith): add a facility to store the error messages for later use? Think errno (but
    // hopefully better)
    try {
        return f();
    }
    RK_EXCEPTION_BOUNDARY_CATCH;
}

// NOTE(sdsmith): Since a variant can't have two duplicate types, specialize return for when the
// functor returns a status.
template <typename F, typename R = std::invoke_result<F>,
          typename = std::enable_if_t<std::is_same_v<R, Status>>>
auto exception_boundary(F&& f) noexcept -> std::variant<Status>
{
    // TODO(sdsmith): add a facility to store the error messages for later use? Think errno (but
    // hopefully better)
    try {
        return f();
    }
    RK_EXCEPTION_BOUNDARY_CATCH;
}

/**
 * \def RK_CHECK_EXB
 * \brief \a RK_CHECK equivalent for \a exception_boundary whose functions don't return a value,
 * only a status code.
 */
#define RK_CHECK_EXB(V)                                                                        \
    do {                                                                                       \
        auto _ret = V;                                                                         \
        auto _p_status = std::get_if<Status>(&_ret);                                           \
        if RK_UNLIKELY (_p_status && *_p_status != ::rk::Status::ok) { RK_CHECK(*_p_status); } \
    } while (0)

// /**
//  * DOC(sdsmith):
//  */
// template <typename T>
// constexpr std::optional<Status> get_exb_status(T const& exb_ret) noexcept
// {
//     if (auto p_status = std::get_if<Status>(&exb_ret)) { return *p_status; }
//     return {};
// }

} // namespace rk
