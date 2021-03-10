#include "core/status.h"

char const* rk::to_string(Status const& status) noexcept
{
    switch (status) {
        case Status::ok: return "ok";
        case Status::generic_error: return "generic_error";
        case Status::api_error: return "api_error";
        case Status::unsupported_function: return "unsupported_function";
        case Status::logic_error: return "logic_error";
        case Status::runtime_error: return "runtime_error";
        case Status::bad_alloc: return "bad_alloc";
        case Status::exception_error: return "exception_error";
        case Status::logger_error: return "logger_error";
        case Status::renderer_error: return "renderer_error";
        case Status::platform_error: return "platform_error";
        case Status::window_error: return "window_error";
        case Status::io_error: return "io_error";
        case Status::invalid_value: return "invalid_value";
    }

    return "unknown";
}
