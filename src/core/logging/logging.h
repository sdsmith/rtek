#pragma once

#include "core/platform/platform.h"
#include "core/utility/status.h"
#include "fmt/printf.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <variant>

#ifdef RK_LOGGING_OFF
//  Remove all logging from the engine. Not recommended!
#    define LOG_INFO(...)
#    define LOG_DEBUG(...)
#    define LOG_WARN(...)
#    define LOG_ERROR(...)
#else
#    ifdef RK_LOGGING_PERF
//      Remove non-critical prints for performance
#        define LOG_INFO(...)
#        define LOG_DEBUG(...)
#    else
#        define LOG_INFO(...) rk::Logger::log_info(RK_FILENAME, __LINE__, RK_FUNCNAME, __VA_ARGS__)
#        define LOG_DEBUG(...) \
            rk::Logger::log_debug(RK_FILENAME, __LINE__, RK_FUNCNAME, __VA_ARGS__)
#    endif
#    define LOG_WARN(...) rk::Logger::log_warn(RK_FILENAME, __LINE__, RK_FUNCNAME, __VA_ARGS__)
#    define LOG_ERROR(...) rk::Logger::log_error(RK_FILENAME, __LINE__, RK_FUNCNAME, __VA_ARGS__)
#endif

// Define the log macros before platform.h so they can be used
#include "core/platform/platform.h"

/*
 * NOTE(sdsmith): can't rely on RK_ASSERT here because it reports to the logger.
 */

namespace rk
{
class Logger {
public:
    static Status initialize() noexcept;

    template <typename... Args>
    static void log_info(char const* file_name, long line_no, char const* func_name,
                         char const* msg, Args const&... args) noexcept
    {
        spdlog_exception_boundary([&]() {
            std::scoped_lock<std::mutex> lock(m_log_mutex);

            if (s_initialized) {
                s_logger->info(sconcat("{}:{}:{}: ", msg).c_str(), file_name, line_no, func_name,
                               args...);
            } else {
                fallback_log(file_name, line_no, func_name, msg, args...);
            }
            return Status::OK;
        });
    }

    template <typename... Args>
    static void log_debug(char const* file_name, long line_no, char const* func_name,
                          char const* msg, Args const&... args) noexcept
    {
        spdlog_exception_boundary([&]() {
            std::scoped_lock<std::mutex> lock(m_log_mutex);

            if (s_initialized) {
                s_logger->debug(sconcat("{}:{}:{}: ", msg).c_str(), file_name, line_no, func_name,
                                args...);
            } else {
                fallback_log(file_name, line_no, func_name, msg, args...);
            }
        });
    }

    template <typename... Args>
    static void log_warn(char const* file_name, long line_no, char const* func_name,
                         char const* msg, Args const&... args) noexcept
    {
        spdlog_exception_boundary([&]() {
            std::scoped_lock<std::mutex> lock(m_log_mutex);

            if (s_initialized) {
                s_logger->warn(sconcat("{}:{}:{}: ", msg).c_str(), file_name, line_no, func_name,
                               args...);
            } else {
                fallback_log_error(file_name, line_no, func_name, msg, args...);
            }
        });
    }

    template <typename... Args>
    static void log_error(char const* file_name, long line_no, char const* func_name,
                          char const* msg, Args const&... args) noexcept
    {
        spdlog_exception_boundary([&]() {
            std::scoped_lock<std::mutex> lock(m_log_mutex);

            if (s_initialized) {
                s_logger->error(sconcat("{}:{}:{}: ", msg).c_str(), file_name, line_no, func_name,
                                args...);
            } else {
                fallback_log_error(file_name, line_no, func_name, msg, args...);
            }
        });
    }

    static void flush() noexcept;

private:
    static bool s_initialized;
    static std::mutex m_log_mutex;

    static std::shared_ptr<spdlog::logger> s_logger;
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
    static void fallback_log(char const* file_name, long line_no, char const* func_name,
                             char const* msg, Args const&... args) noexcept
    {
        spdlog_exception_boundary([&]() {
            fmt::fprintf(std::cout, sconcat("{}:{}:{}: ", msg).c_str(), file_name, line_no,
                         func_name,
                         args...); //-V111
        });
    }

    template <typename... Args>
    static void fallback_log_error(char const* file_name, long line_no, char const* func_name,
                                   char const* msg, Args const&... args) noexcept
    {
        spdlog_exception_boundary([&]() {
            fmt::fprintf(std::cerr, sconcat("{}:{}:{}: ", msg).c_str(), file_name, line_no,
                         func_name,
                         args...); //-V111
        });
    }

    static void fallback_flush() noexcept;
};
} // namespace rk
