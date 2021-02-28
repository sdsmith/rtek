#include "core/logging/logging.h"

#include "core/utility/assert.h"
#include "spdlog/spdlog.h"
#include <string>

using namespace rk;

bool Logger::s_initialized = false;
std::mutex Logger::m_fallback_log_mutex;
std::string Logger::s_log_dir = "log";
std::string Logger::s_log_file = "rtek.log";

Status Logger::initialize() noexcept
{
    using namespace platform;

    // Create log directory
    if (!directory_exists(s_log_dir.c_str())) { RK_CHECK(create_directory(s_log_dir.c_str())); }

    Status status = Status::ok;
    spdlog_exception_boundary([&]() {
        try {
            auto stderr_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
            stderr_sink->set_level(spdlog::level::err);

            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                s_log_dir + "/" + s_log_file, true);
            file_sink->set_level(spdlog::level::trace);

            std::array<spdlog::sink_ptr, 2> sinks = {stderr_sink, file_sink};
            spdlog::set_default_logger(
                std::make_shared<spdlog::logger>("rtek", sinks.begin(), sinks.end()));
            spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%F] [%l] %s:%#:%!: %v");

            s_initialized = true;

        } catch (const spdlog::spdlog_ex& e) {
            std::cerr << "Unable to initialize logger: " << e.what() << '\n';
            throw;
        }
    });

    return status;
}

void Logger::flush() noexcept
{
    spdlog_exception_boundary([&]() {
        if (s_initialized) { spdlog::default_logger_raw()->flush(); }
        fallback_flush();
    });
}

std::string Logger::sconcat(char const* a, char const* b) noexcept(false)
{
    assert(a);
    assert(b);
    return std::string(a) + std::string(b);
}

void Logger::fallback_flush() noexcept
{
    std::cout << std::flush;
    std::cerr << std::flush;
}
