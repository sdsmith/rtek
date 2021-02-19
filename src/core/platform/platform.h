#pragma once

#include "core/types.h"
#include "core/utility/status.h"
#include <cstring>

#if RK_OS == RK_OS_WINDOWS
#   define NOMINMAX      // Stop definition of max and min from windows.h
#   include <Windows.h>
#   include <strsafe.h>

#   define PATH_SEPARATOR '\\'

namespace rk
{
    namespace Platform
    {
        void log_windows_error(const LPCTSTR function_name);

        Status create_directory(char const* directory);
        bool directory_exists(char const* path);
    }
}

#elif defined(__CYGWIN__) && RK_OS != RK_OS_WINDOWS
#   warning Cygwin is not officially supported
#else
#   error Unsupported platform
#   define PATH_SEPARATOR '/'
#endif

#define _FILENAME_ (std::strrchr(__FILE__, PATH_SEPARATOR) ? std::strrchr(__FILE__, PATH_SEPARATOR) + 1 : __FILE__)
