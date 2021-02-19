#pragma once

#include "core/core.h"
#include "core/utility/status.h"
#include "core/utility/version.h"

namespace rk
{
constexpr const char* name = "RTEK";
constexpr Version version = {0, 0};

Status initialize() { return Rtek_Engine::initialize(); }
Status destroy() { return Rtek_Engine::destroy(); }
Status run() { return Rtek_Engine::run(); }
} // namespace rk
