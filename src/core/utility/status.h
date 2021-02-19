#pragma once

#include "core/types.h"
#include "core/utility/status.h"

namespace rk
{
    class Status
    {
    public:
        enum Type : s32
        {
            OK = 0,
            GENERIC_ERROR = 1,
            API_ERROR = 2,
            UNSUPPORTED_FUNCTION = 3
        };

        constexpr Status(Type status) : m_status(status) {}

        operator s32() const { return m_status; }

    private:
        Type m_status;
    };

    inline bool StatusCheck(Status status) {
        return status == Status::OK;
    }
}

#define RTK_CHECK(V)                        \
    do {                                    \
        ::rk::Status status_ = V;         \
        if (!::rk::StatusCheck(status_)) { \
            return status_;                 \
        }                                   \
    } while (0)
