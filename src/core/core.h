#pragma once

#include "core/types.h"
#include "core/utility/status.h"

namespace Rtek
{
    class RtekEngine
    {
    public:
        static Status Initialize();
        static Status Destroy();

        static Status Run();

    private:
        static bool m_initialized;
    };
}
