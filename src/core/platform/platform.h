#pragma once

#include "core/status.h"
#include "core/types.h"
#include <cstring>

#if RK_OS == RK_OS_WINDOWS
#    define NOMINMAX // Stop definition of max and min from windows.h
#    include <Windows.h>
#    include <strsafe.h>

#    define RK_PATH_SEPARATOR "\\"

namespace rk::platform
{
// TODO(sdsmith): probs shouldn't be in platform
s32 ustrlen(uchar const* s) noexcept;

namespace windows
{
void log_error(HRESULT hresult) noexcept;

// TODO(sdsmith): uchar param or regular char param?
void log_last_error(uchar const* function_name);
} // namespace windows

} // namespace rk::platform

#elif defined(__CYGWIN__) && RK_OS != RK_OS_WINDOWS
#    warning Cygwin is not officially supported
#else
#    error Unsupported platform
#    define RK_PATH_SEPARATOR "/"
#endif
