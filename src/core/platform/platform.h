#pragma once

#include "core/status.h"
#include "core/types.h"
#include <cstring>

#if RK_OS == RK_OS_WINDOWS
#    include "core/platform/win32_include.h"

namespace rk::platform::windows
{
void log_error(HRESULT hresult) noexcept;

// TODO(sdsmith): uchar param or regular char param?
void log_last_error(uchar const* function_name);

} // namespace rk::platform::windows

#elif defined(__CYGWIN__) && RK_OS != RK_OS_WINDOWS
#    warning Cygwin is not officially supported
#else
#    error Unsupported platform
#endif
