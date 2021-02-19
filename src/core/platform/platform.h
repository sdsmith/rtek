#pragma once

#include "core/utility/status.h"
#include <cstring>

#if defined(_WIN32) || defined(_WIN64)
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

#elif defined(__CYGWIN__) && !defined(_WIN32)
#   warning Cygwin is not officially supported
#else
#   error Unsupported platform
#   define PATH_SEPARATOR '/'
#endif

#define _FILENAME_ (std::strrchr(__FILE__, PATH_SEPARATOR) ? std::strrchr(__FILE__, PATH_SEPARATOR) + 1 : __FILE__)
