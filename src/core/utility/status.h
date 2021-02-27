#pragma once

#include "core/types.h"
#include "core/utility/status.h"

namespace rk
{
class Status {
public:
    enum Type : s32 {
        OK = 0,
        GENERIC_ERROR = 1,
        API_ERROR = 2,
        UNSUPPORTED_FUNCTION = 3,
        LOGIC_ERROR = 4,
        RUNTIME_ERROR = 5,
        BAD_ALLOC = 6,
        EXCEPTION_ERROR = 7,
        LOGGER_ERROR = 8,
    };

    constexpr Status(Type status) : m_status(status) {}

    operator s32() const { return m_status; }

private:
    Type m_status = GENERIC_ERROR;
};

RK_INTERNAL
inline bool status_check(Status status) { return status == Status::OK; }
} // namespace rk

#define RK_CHECK(V)                                           \
    do {                                                      \
        ::rk::Status status_ = V;                             \
        if (!::rk::status_check(status_)) { return status_; } \
    } while (0)
