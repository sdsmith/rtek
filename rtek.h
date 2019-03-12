#pragma once

#include "src/core/core.h"
#include "src/core/version.h"

namespace Rtek
{
    constexpr const char* name = "RTEK";
    constexpr Version version = { 0, 0 };

    Status Initialize() { return RtekEngine::Initialize(); }
    Status Destroy() { return RtekEngine::Destroy(); }
    Status Run() { return RtekEngine::Run(); }
}
