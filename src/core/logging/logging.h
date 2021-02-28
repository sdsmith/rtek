#pragma once

/*
 * NOTE(sdsmith): Can't rely on RK_ASSERT here because it reports to the logger.
 */

#include "core/platform/platform.h"
#include "core/utility/status.h"
#include "fmt/chrono.h"
#include "fmt/core.h"

#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <variant>

//******************************************************************************
// Setup logging level. Unused logging levels are not compiled in.
//

/**
 * \def RK_LOG_LEVEL
 * \brief Log level to compile into the application.
 *
 * Compiles in log level >= the given log level. Accepts values from
 * `RK_LOG_LEVEL_*`.
 *
 * CLI usage: `-DRK_LOG_LEVEL=RK_LOG_LEVEL_INFO`
 */

/**
 * \def RK_LOGGING_OFF
 * \define Remove all logging.
 *
 * Takes precedence over \ref RK_LOG_LEVEL.
 */

/**
 * \def RK_LOGGING_PERF
 * \define Remove all non-detailed logging.
 *
 * Takes precedence over \ref RK_LOG_LEVEL.
 */

#define RK_LOG_LEVEL_OFF SPDLOG_LEVEL_OFF
#define RK_LOG_LEVEL_TRACE SPDLOG_LEVEL_TRACE
#define RK_LOG_LEVEL_DEBUG SPDLOG_LEVEL_DEBUG
#define RK_LOG_LEVEL_INFO SPDLOG_LEVEL_INFO
#define RK_LOG_LEVEL_WARN SPDLOG_LEVEL_WARN
#define RK_LOG_LEVEL_ERROR SPDLOG_LEVEL_ERROR
#define RK_LOG_LEVEL_CRITICAL SPDLOG_LEVEL_CRITICAL

#ifdef RK_LOGGING_OFF
#    define RK_LOG_LEVEL RK_LOG_LEVEL_OFF
#elif defined(RK_LOGGING_PERF)
#    define RK_LOG_LEVEL SPDLOG_LEVEL_WARN
#endif

// If `RK_LOG_LEVEL` wasn't suppurceeded and isn't defined, set to default.
#ifndef RK_LOG_LEVEL
#    define RK_LOG_LEVEL RK_LOG_LEVEL_INFO
#endif

// NOTE(sdsmith): SPDLOG_ACTIVE_LEVEL must be set before spdlog.h is included.
#define SPDLOG_ACTIVE_LEVEL RK_LOG_LEVEL
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

// Use the follow macros for logging
//

/*
 * NOTE(sdsmith): Can't use the SPDLOG_* (ex. SPDLOG_INFO) macros directly. It would be easy, but
 * would leak exceptions. In order to isolate exceptions to this translation unit, need to have a
 * function as an indirection point (macro -> function -> spdlog).
 */

#if RK_LOG_LEVEL <= RK_LOG_LEVEL_TRACE
#    define LOG_TRACE(...) \
        ::rk::Logger::log(spdlog::level::trace, RK_FILENAME, __LINE__, RK_FUNCNAME, __VA_ARGS__)
#else
#    define LOG_TRACE(...) static_cast<void>(0)
#endif

#if RK_LOG_LEVEL <= RK_LOG_LEVEL_INFO
#    define LOG_INFO(...) \
        ::rk::Logger::log(spdlog::level::info, RK_FILENAME, __LINE__, RK_FUNCNAME, __VA_ARGS__)
#else
#    define LOG_INFO(...) static_cast<void>(0)
#endif

#if RK_LOG_LEVEL <= RK_LOG_LEVEL_DEBUG
#    define LOG_DEBUG(...) \
        ::rk::Logger::log(spdlog::level::debug, RK_FILENAME, __LINE__, RK_FUNCNAME, __VA_ARGS__)
#else
#    define LOG_DEBUG(...) static_cast<void>(0)
#endif

#if RK_LOG_LEVEL <= RK_LOG_LEVEL_WARN
#    define LOG_WARN(...) \
        ::rk::Logger::log(spdlog::level::warn, RK_FILENAME, __LINE__, RK_FUNCNAME, __VA_ARGS__)
#else
#    define LOG_WARN(...) static_cast<void>(0)
#endif

#if RK_LOG_LEVEL <= RK_LOG_LEVEL_ERROR
#    define LOG_ERROR(...) \
        ::rk::Logger::log(spdlog::level::err, RK_FILENAME, __LINE__, RK_FUNCNAME, __VA_ARGS__)
#else
#    define LOG_ERROR(...) static_cast<void>(0)
#endif

#if RK_LOG_LEVEL <= RK_LOG_LEVEL_CRITICAL
#    define LOG_CRITICAL(...)                                                                   \
        ::rk::Logger::log_critical(spdlog::level::critical, RK_FILENAME, __LINE__, RK_FUNCNAME, \
                                   __VA_ARGS__)
#else
#    define LOG_CRITICAL(...) static_cast<void>(0)
#endif

//******************************************************************************

// Define the log macros before platform.h so they can be used
#include "core/platform/platform.h"

namespace rk
{
class Logger {
public:
    static Status initialize() noexcept;

    template <typename... Args>
    static void log(spdlog::level::level_enum level, char const* file_name, long line_no,
                    char const* func_name, Args const&... args) noexcept
    {
        assert(file_name);
        assert(func_name);

        if (s_initialized) {
            spdlog_exception_boundary([&]() {
                spdlog::default_logger_raw()->log(spdlog::source_loc{file_name, line_no, func_name},
                                                  level, args...);
            });
        } else if (level < spdlog::level::warn) {
            fallback_log_info(file_name, line_no, func_name, args...);
        } else {
            fallback_log_error(file_name, line_no, func_name, args...);
        }
    }

    static void flush() noexcept;

private:
    static bool s_initialized;
    static std::mutex m_fallback_log_mutex;
    static std::string s_log_dir;
    static std::string s_log_file;

    static std::string sconcat(char const* a, char const* b) noexcept(false);

    /**
     * \brief Exception boundary for spdlog.
     *
     * Unlike the rest of the source, spdlog log can't log it's errors to the logger because it
     * is the logger. In the case of failure, errors should be logged to stderr.
     *
     * \see rk::exception_boundary
     */
    template <typename Func>
    static void spdlog_exception_boundary(Func&& f) noexcept
    {
        try {
            f();
            return;
        } catch (const spdlog::spdlog_ex& e) {
            std::cerr << e.what() << '\n';
        } catch (const std::exception& e) {
            std::cerr << "Unhandled exception: " << e.what() << '\n';
        } catch (...) {
            std::cerr << "Unknown exception occured\n";
        }

        // TODO(sdsmith): There isn't really much that can be done here since it's the logger...
        std::terminate();
    }

    template <typename... Args>
    static void fallback_log(std::FILE* f, char const* file_name, long line_no,
                             char const* func_name, Args const&... args) noexcept
    {
        assert(file_name);
        assert(func_name);

        // TODO(sdsmith): Can't get fmt::print to compile. Not sure why because
        // the MSVC compilation messages are so poor...
        assert(!"Doesn't work!");

        // spdlog_exception_boundary([&]() {
        //     const std::time_t t = std::time(nullptr);
        //     std::scoped_lock<std::mutex> lock(m_fallback_log_mutex);
        //     fmt::print(f, "[{:%Y-%m-%d %H:%M:%S}] <fallback logger> {}:{}:{}: ",
        //     fmt::localtime(t),
        //                file_name, line_no, func_name);

        //     if constexpr (sizeof...(Args) == 1) {
        //         fmt::print(f, "{}", args...);
        //     } else {
        //         fmt::print(f, args...);
        //     }
        //     fmt::print(f, "\n");
        // });
    }

    template <typename... Args>
    static void fallback_log_info(char const* file_name, long line_no, char const* func_name,
                                  Args const&... args) noexcept
    {
        fallback_log(stdout, file_name, line_no, func_name, args...);
    }

    template <typename... Args>
    static void fallback_log_error(char const* file_name, long line_no, char const* func_name,
                                   char const* msg, Args const&... args) noexcept
    {
        fallback_log(stderr, file_name, line_no, func_name, args...);
    }

    static void fallback_flush() noexcept;
};
} // namespace rk
