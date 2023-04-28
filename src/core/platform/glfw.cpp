#include "core/platform/glfw.h"

#include "core/logging/logging.h"

using namespace rk;
using namespace sds;

/**
 * \brief Called on every glfw error when hooked up with \a glfwSetErrorCallback.
 */
RK_INTERNAL
void glfw_error_callback(s32 error_code, char const* description)
{
    // TODO(sdsmith): if using GLFW from multiple threads, this needs to support that
    RK_ASSERT(description);
    RK_ASSERT(error_code != GLFW_NO_ERROR);
    LOG_ERROR("glfw failed with error code {:#010x}: {}", error_code, description);
}

Status platform::glfw::handle_error() noexcept
{
    switch (glfwGetError(nullptr)) {
        case GLFW_NO_ERROR: return Status::ok;
        case GLFW_INVALID_VALUE: return Status::invalid_value;
        default: platform::glfw::destroy(); return Status::platform_error;
    }
}

// TODO(sdsmith): make class and keep an m_initialized bool around
Status platform::glfw::initialize() noexcept
{
    glfwSetErrorCallback(glfw_error_callback);
    if (glfwInit() == GLFW_FALSE) {
        // NOTE(sdsmith): glfwInit calls glfwTerminate on failure.
        LOG_CRITICAL("Failed to initialize glfw");
        return Status::platform_error;
    }

    return Status::ok;
}

Status platform::glfw::destroy() noexcept
{
    glfwTerminate();
    return Status::ok;
}
