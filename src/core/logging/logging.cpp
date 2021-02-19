#include "core/logging/logging.h"

#include "core/utility/assert.h"
#include <string>

using namespace rk;

bool Logger::s_initialized = false;
std::mutex Logger::m_log_mutex;
std::shared_ptr<spdlog::logger> Logger::s_logger{nullptr};
std::string Logger::s_log_dir = "log";
std::string Logger::s_log_file = "rtek.log";

Status Logger::initialize() noexcept
{
    using namespace Platform;

    // Create log directory
    if (!directory_exists(s_log_dir.c_str())) { RK_CHECK(create_directory(s_log_dir.c_str())); }

    try {
        auto stderr_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
        stderr_sink->set_level(spdlog::level::err);

        auto file_sink =
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(s_log_dir + "/" + s_log_file, true);
        file_sink->set_level(spdlog::level::trace);

        std::vector<spdlog::sink_ptr> sinks = {stderr_sink, file_sink};
        s_logger = std::make_shared<spdlog::logger>("rtek", sinks.begin(), sinks.end());

        s_initialized = true;

    } catch (spdlog::spdlog_ex&) {
        std::cerr << "Unable to initialize logger\n";
        return Status::GENERIC_ERROR;
    }

    return Status::OK;
}

void Logger::flush() noexcept
{
    if (s_initialized) { s_logger->flush(); }

    fallback_flush();
}

std::string Logger::sconcat(char const* a, char const* b)
{
    RK_ASSERT(a);
    RK_ASSERT(b);
    return std::string(a) + std::string(b);
}

void Logger::fallback_flush() noexcept
{
    std::cout << std::flush;
    std::cerr << std::flush;
}
