#pragma once

#include "src/core/types.h"
#include "src/core/status.h"

namespace Rtek
{
    class Status
    {
    public:
        enum Type : s32
        {
            OK = 0,
            GENERIC_ERROR = 1,
            API_ERROR = 2,
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
        ::Rtek::Status status_ = V;         \
        if (!::Rtek::StatusCheck(status_)) { \
            return status_;                 \
        }                                   \
    } while (0)
