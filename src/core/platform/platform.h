#pragma once

#include "core/status.h"
#include "core/types.h"
#include <cstring>

#if SDS_OS_WINDOWS
#    include "core/platform/win32_include.h"

namespace rk::platform::windows
{
void log_error(HRESULT hresult) noexcept;
void log_last_error(char const* file_name, int line_no,
    char const* func_name, char const* platform_func_name) noexcept;
} // namespace rk::platform::windows

#elif defined(__CYGWIN__) && SDS_OS != SDS_OS_WINDOWS
#    warning Cygwin is not officially supported

#elif SDS_OS_LINUX

namespace rk::platform::linux
{
void log_last_error(char const* file_name, int line_no,
    char const* func_name, char const* platform_func_name) noexcept;
} // namespace rk::platform::linux

#else
#    error Unsupported platform
#endif


#if SDS_OS_WINDOWS
#   define LOG_OS_LAST_ERROR(OS_FUNC) \
        ::rk::platform::windows::log_last_error(RK_FILENAME, __LINE__, RK_FUNCNAME, OS_FUNC)
#elif SDS_OS_LINUX
#   define LOG_OS_LAST_ERROR(OS_FUNC) \
        ::rk::platform::linux::log_last_error(RK_FILENAME, __LINE__, RK_FUNCNAME, OS_FUNC)
#else
#   error Unsupported platform
#endif