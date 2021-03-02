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

// Check that glfw primitive types match the engines
RK_STATIC_ASSERT_MSG(sizeof(int) == sizeof(s32), "glfw sizes don't match");

/**
 * \brief Window resize callback function.
 */
void framebuffer_size_callback(GLFWwindow* window, s32 width, s32 height)
{
    glViewport(0, 0, width, height);
}

Status Rtek_Engine::initialize()
{
    RK_CHECK(Logger::initialize());
    LOG_INFO("Logger initialized");
    LOG_INFO("Initializing engine...");

    LOG_INFO("Initializing OpenGL context...");
    glfwInit();

    constexpr s32 glctx_ver_major = 4;
    constexpr s32 glctx_ver_minor = 6;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glctx_ver_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glctx_ver_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    constexpr s32 window_w = 800;
    constexpr s32 window_h = 600;
    GLFWwindow* window = glfwCreateWindow(window_w, window_h, "RTek", nullptr, nullptr);
    if (!window) {
        LOG_CRITICAL("Failed to create window");
        glfwTerminate(); // TODO(sdsmith): this should be done any error after glfw is initialized
        return Status::renderer_error;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD with the OS-specific OGL func pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        LOG_CRITICAL("Failed to initialize GLAD");
        return Status::renderer_error;
    }
    glViewport(0, 0, window_w, window_h);
    LOG_INFO("OpenGL context initialized: OpenGL {}.{} core", glctx_ver_major, glctx_ver_minor);

    // TODO(sdsmith): move to `run`
    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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

    glfwTerminate();

    m_initialized = false;
    LOG_INFO("Engine destroyed");
    Logger::flush();
    return Status::ok;
}

Status Rtek_Engine::run() { return Status::ok; }

bool Rtek_Engine::m_initialized = false;
