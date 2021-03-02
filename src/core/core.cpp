#include "core/core.h"

#include "core/logging/logging.h"
#include "core/utility/assert.h"

// clang-format off
// NOTE(sdsmith): GLAD must be included before any other OpenGL related header.
// GLAD includes the OpenGL headers.
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

using namespace rk;

Status Rtek_Engine::initialize()
{
    RK_CHECK(Logger::initialize());
    LOG_INFO("Logger initialized");
    LOG_INFO("Initializing engine...");

    LOG_INFO("Initializing OpenGL context...");
    constexpr s32 glctx_ver_major = 4;
    constexpr s32 glctx_ver_minor = 6;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glctx_ver_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glctx_ver_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    LOG_INFO("OpenGL context initialized: OpenGL {}.{} core", glctx_ver_major, glctx_ver_minor);

    LOG_INFO("Engine initialized");
    m_initialized = true;
    return Status::ok;
}

Status Rtek_Engine::destroy()
{
    if (!m_initialized) {
        LOG_ERROR("Attempt to destroy uninitialized engine");
        return Status::api_error;
    }

    m_initialized = false;

    LOG_INFO("Engine destroyed");
    Logger::flush();
    return Status::ok;
}

Status Rtek_Engine::run() { return Status::ok; }

bool Rtek_Engine::m_initialized = false;
