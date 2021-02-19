#pragma once

#include "core/types.h"
#include "core/utility/status.h"

namespace rk
{
    class Rtek_Engine
    {
    public:
        static Status initialize();
        static Status destroy();

        static Status run();

    private:
        static bool m_initialized;
    };
}
