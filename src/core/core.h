#pragma once

#include "src/core/status.h"
#include "src/core/types.h"

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
