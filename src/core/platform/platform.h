#pragma once

#include "core/status.h"
#include "core/types.h"
#include <cstring>

#if RK_OS == RK_OS_WINDOWS
#    include "core/platform/win32_include.h"

namespace rk::platform::windows
{
void log_error(HRESULT hresult) noexcept;
void log_last_error(char const* function_name) noexcept;

} // namespace rk::platform::windows

#elif defined(__CYGWIN__) && RK_OS != RK_OS_WINDOWS
#    warning Cygwin is not officially supported
#else
#    error Unsupported platform
#endif
