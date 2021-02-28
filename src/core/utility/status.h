#pragma once

#include "core/types.h"
#include "core/utility/status.h"

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
    };

    constexpr Status(Type status) : m_status(status) {}

    operator s32() const { return m_status; }

private:
    Type m_status = generic_error;
};

RK_INTERNAL
inline bool status_check(Status status) { return status == Status::ok; }
} // namespace rk

#define RK_CHECK(V)                                           \
    do {                                                      \
        ::rk::Status status_ = V;                             \
        if (!::rk::status_check(status_)) { return status_; } \
    } while (0)
