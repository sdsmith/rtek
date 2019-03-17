#pragma once

#include "core/core.h"
#include "core/utility/status.h"
#include "core/utility/version.h"

namespace Rtek
{
    constexpr const char* name = "RTEK";
    constexpr Version version = { 0, 0 };

    Status Initialize() { return RtekEngine::Initialize(); }
    Status Destroy() { return RtekEngine::Destroy(); }
    Status Run() { return RtekEngine::Run(); }
}
