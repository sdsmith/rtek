#pragma once

#include "core/types.h"

namespace rk
{
class Status {
public:
    enum Type : s32 {
        ok = 0,
        generic_error = 1,
        api_error = 2,
        unsupported_function = 3,
        logic_error = 4,
        runtime_error = 5,
        bad_alloc = 6,
        exception_error = 7,
        logger_error = 8,
        renderer_error = 9,
        platform_error = 10,
        window_error = 11,
        io_error = 12,
        invalid_value = 13,
        unicode_error = 14,
    };

    constexpr Status(Type status) : m_status(status) {}

    operator s32() const { return m_status; }

private:
    Type m_status = generic_error;
};

char const* to_string(Status const& status) noexcept;

RK_INTERNAL
inline bool status_check(Status status) { return status == Status::ok; }
} // namespace rk

#define RK_CHECK(V)                                           \
    do {                                                      \
        ::rk::Status status_ = V;                             \
        if (!::rk::status_check(status_)) { return status_; } \
    } while (0)
