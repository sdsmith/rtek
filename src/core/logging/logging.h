#pragma once

/*
 * NOTE(sdsmith): Can't rely on RK_ASSERT here because it reports to the logger.
 */

#include "core/platform/platform.h"
#include "core/platform/stdlib/iostream.h"
#include "core/platform/unicode.h"
#include "core/status.h"
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <sds/type_traits.h>
#include <cassert>
#include <exception>
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

#define RK_LOG_LEVEL_TRACE 0
#define RK_LOG_LEVEL_DEBUG 1
#define RK_LOG_LEVEL_INFO 2
#define RK_LOG_LEVEL_WARN 3
#define RK_LOG_LEVEL_ERROR 4
#define RK_LOG_LEVEL_CRITICAL 5
#define RK_LOG_LEVEL_OFF 6

#if defined(RK_LOGGING_OFF) && defined(RK_LOGGING_PERF)
#    error Must defined either RK_LOGGING_OFF or RK_LOGGING_PERF. Not both.
#endif

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
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

// Check the definitions are in sync
static_assert(RK_LOG_LEVEL_TRACE == SPDLOG_LEVEL_TRACE);
static_assert(RK_LOG_LEVEL_DEBUG == SPDLOG_LEVEL_DEBUG);
static_assert(RK_LOG_LEVEL_INFO == SPDLOG_LEVEL_INFO);
static_assert(RK_LOG_LEVEL_WARN == SPDLOG_LEVEL_WARN);
static_assert(RK_LOG_LEVEL_ERROR == SPDLOG_LEVEL_ERROR);
static_assert(RK_LOG_LEVEL_CRITICAL == SPDLOG_LEVEL_CRITICAL);
static_assert(RK_LOG_LEVEL_OFF == SPDLOG_LEVEL_OFF);

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
#    define LOG_CRITICAL(...) \
        ::rk::Logger::log(spdlog::level::critical, RK_FILENAME, __LINE__, RK_FUNCNAME, __VA_ARGS__)
#else
#    define LOG_CRITICAL(...) static_cast<void>(0)
#endif

//******************************************************************************

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
        } else {
            fallback_log(file_name, line_no, func_name, level, args...);
        }
    }

    static void flush() noexcept;

    static void emergency_shutdown() noexcept
    {
        // Flush async loggers manually is done as part of spdlog shutdown. Expecting immediate
        // shutdown and no more logs!
        // ref: https://github.com/gabime/spdlog/wiki/7.-Flush-policy
        spdlog::shutdown();

        s_initialized = false;
    }

private:
    static bool s_initialized;
    static std::mutex m_fallback_log_mutex;
    static char const* s_log_dir;
    static char const* s_log_file;

    static std::string sconcat(char const* a, char const* b) noexcept(false);

    static spdlog::logger* get_default_logger() noexcept;

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
            rk::cerr << e.what() << '\n';
        } catch (const std::exception& e) {
            rk::cerr << "Unhandled exception: " << e.what() << '\n';
        } catch (...) {
            rk::cerr << "Unknown exception occured\n";
        }

        // TODO(sdsmith): There isn't really much that can be done here since it's the logger...
        std::terminate();
    }

    using fmt_wstring_view = fmt::basic_string_view<wchar_t>;

    /**
     * \brief Fallback logging for when the primary logger is not initalized.
     *
     * NOTE(sdsmith): This overload handles wide encoded strings.
     */
    template <typename... Args>
    static void fallback_log(char const* file_name, long line_no, char const* func_name,
                             spdlog::level::level_enum level, fmt_wstring_view fmt,
                             Args&&... args) noexcept
    {
        // fmtlib has a horrible error message for encoding mismatches. Use this to improve the
        // error message.
        static_assert(!sds::contains<char const*, std::decay_t<Args>...>::value,
                      "Cannot mix narrow and wide encodings for format and parameter strings");
        static_assert(!sds::contains<char*, std::decay_t<Args>...>::value,
                      "Cannot mix narrow and wide encodings for format and parameter strings");

        assert(file_name);
        assert(func_name);

        std::FILE* f = (level < spdlog::level::warn ? stdout : stderr);

        // TODO(sdsmith): convert stuff to utf-8 before it goes out
        spdlog_exception_boundary([&]() {
            const std::time_t t = std::time(nullptr);
            std::scoped_lock<std::mutex> lock(m_fallback_log_mutex);

            std::string const loc = fmt::format("{}:{}:{}", file_name, line_no, func_name);
            std::wstring w_loc = unicode::widen(loc);
            fmt::print(f, L"[{:%Y-%m-%d %H:%M:%S}] <fallback logger> {}: \n", fmt::localtime(t),
                       w_loc);

            fmt::wmemory_buffer wbuf;
            fmt::format_to(wbuf, fmt, std::forward<Args>(args)...);
            fmt::print(L"{}\n", wbuf.data());
        });
    }

    using fmt_string_view = fmt::basic_string_view<char>;

    /**
     * \brief Fallback logging for when the primary logger is not initalized.
     */
    template <typename... Args>
    static void fallback_log(char const* file_name, long line_no, char const* func_name,
                             spdlog::level::level_enum level, fmt_string_view fmt,
                             Args&&... args) noexcept
    {
        // fmtlib has a horrible error message for encoding mismatches. Use this to improve the
        // error message.
        static_assert(!sds::contains<wchar_t const*, std::decay_t<Args>...>::value,
                      "Cannot mix narrow and wide encodings for format and parameter strings");
        static_assert(!sds::contains<wchar_t*, std::decay_t<Args>...>::value,
                      "Cannot mix narrow and wide encodings for format and parameter strings");

        assert(file_name);
        assert(func_name);

        std::FILE* f = (level < spdlog::level::warn ? stdout : stderr);

        spdlog_exception_boundary([&]() {
            const std::time_t t = std::time(nullptr);
            std::scoped_lock<std::mutex> lock(m_fallback_log_mutex);

            fmt::print(f, "[{:%Y-%m-%d %H:%M:%S}] <fallback logger> {}:{}:{}: \n",
                       fmt::localtime(t), file_name, line_no, func_name);

            fmt::memory_buffer buf;
            fmt::format_to(buf, fmt, std::forward<Args>(args)...);
            fmt::print("{}\n", buf.data());
        });
    }

    static void fallback_flush() noexcept;
};
} // namespace rk
