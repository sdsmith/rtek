#pragma once

#include "src/core/platform.h"

namespace Rtek
{
    void LogInfo(char const* file_name, long line_no, char const* func_name, char const* msg, ...);
    void LogDebug(char const* file_name, long line_no, char const* func_name, char const* msg, ...);
    void LogWarn(char const* file_name, long line_no, char const* func_name, char const* msg, ...);
    void LogError(char const* file_name, long line_no, char const* func_name, char const* msg, ...);
}


#ifdef RTK_LOGGING_OFF
//  Remove all logging from the engine. Not recommended!
#   define LOG_INFO(...)
#   define LOG_DEBUG(...)
#   define LOG_WARN(...)
#   define LOG_ERROR(...)
#else
#   ifdef RTK_LOGGING_PERF
//      Remove non-critical prints for performance
#       define LOG_INFO(...)
#       define LOG_DEBUG(...)
#   else
#       define LOG_INFO(...)  LogInfo(__FILE__, __LINE__, RTK_FUNC_NAME, __VA_ARGS__)
#       define LOG_DEBUG(...) LogDebug(__FILE__, __LINE__, RTK_FUNC_NAME, __VA_ARGS__)
#   endif
#   define LOG_WARN(...) LogWarn(__FILE__, __LINE__, RTK_FUNC_NAME, __VA_ARGS__)
#   define LOG_ERROR(...) LogError(__FILE__, __LINE__, RTK_FUNC_NAME, __VA_ARGS__)
#endif
