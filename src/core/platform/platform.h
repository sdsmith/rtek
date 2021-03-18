#pragma once

#include "core/status.h"
#include "core/types.h"
#include <cstring>

#if RK_OS == RK_OS_WINDOWS
#    define NOMINMAX // Stop definition of max and min from windows.h
#    include <Windows.h>
#    include <strsafe.h>

namespace rk::platform
{
/**
 * \brief Number of characters in the string.
 *
 * For wide encoded strings, this is the number of wide chracters.
 * For multi-byte encoded strings this is the number of encoded characters, not the number of bytes.
 */
// TODO(sdsmith): move to rk::unicode
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
#endif
