#pragma once

#include "core/logging/logging.h"
#include "core/utility/status.h"
#include <exception>
#include <variant>

namespace rk
{
/**
 * \brief Stops all exceptions and converts them to status codes.
 *
 * Usage: DOC(sdsmith): TODO(sdsmith):
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
template <typename F>
auto exception_boundary(F&& f) noexcept -> std::variant<decltype(f()), Status>
{
    // TODO(sdsmith): add a facility to store the error messages for later use? Think errno (but
    // hopefully better)
    try {
        return f();
    } catch (const std::logic_error& e) {
        LOG_ERROR(e.what());
        return Status::LOGIC_ERROR;
    } catch (const std::runtime_error& e) {
        LOG_ERROR(e.what());
        return Status::RUNTIME_ERROR;
    } catch (const std::bad_alloc& e) {
        LOG_ERROR(e.what());
        return Status::BAD_ALLOC;
    } catch (const std::bad_exception& e) {
        LOG_ERROR(e.what());
        return Status::EXCEPTION_ERROR;
    } catch (const std::exception& e) {
        LOG_ERROR("Unhandled exception: {}", e.what());
        return Status::GENERIC_ERROR;
    } catch (...) {
        LOG_ERROR("Unknown exception occured");
        return Status::GENERIC_ERROR;
    }
}

} // namespace rk
