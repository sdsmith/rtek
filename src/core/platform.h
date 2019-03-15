#pragma once

#include "src/core/status.h"
#include <cstring>

#if defined(_WIN32) || defined(_WIN64)
#   define NOMINMAX      // Stop definition of max and min from windows.h
#   include <Windows.h>
#   include <strsafe.h>

namespace Rtek
{
    namespace Platform
    {
        void log_windows_error(const LPCTSTR function_name);

        Status create_directory(char const* directory);
        bool directory_exists(char const* path);
    }
}

#elif defined(__CYGWIN__) && !defined(_WIN32)
#   warning Cygwin is not officially supported
#else
#   error Unsupported platform
#endif

#if defined(_MSC_VER)
//  Compiling with MSVC
#   define RTK_FUNC_NAME __FUNCTION__
#else
#   define RTK_FUNC_NAME __FUNC__
#endif
