#include "core/renderer/renderer.h"

#include "core/logging/logging.h"
#include "core/utility/assert.h"
#include "fmt/core.h"

using namespace rk;

Status Renderer::initialize() noexcept { return Status::ok; }

Status Renderer::destroy() noexcept { return Status::ok; }

Status Renderer::prepare_window() noexcept
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_opengl_ctx_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_opengl_ctx_version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return platform::glfw::handle_error();
}

void Renderer::set_window(GLFWwindow* window) noexcept
{
    RK_ASSERT(window);
    m_window = window;
}

// TODO(sdsmith): this feels very platform-y
Status Renderer::setup_gl_api()
{
    // Initialize GLAD with the OS-specific OpenGL function pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        LOG_CRITICAL("Failed to initialize GLAD");
        return Status::renderer_error;
    }

    s32 window_w = -1;
    s32 window_h = -1;
    glfwGetWindowSize(m_window, &window_w, &window_h);
    RK_CHECK(platform::glfw::handle_error());

    glViewport(0, 0, window_w, window_h);

    LOG_INFO("OpenGL context initialized: {}", get_gl_api_version());
    return Status::ok;
}

Status Renderer::swap_buffers() const noexcept
{
    glfwSwapBuffers(m_window);
    return platform::glfw::handle_error();
}

/**
 * \brief Window resize callback function.
 */
RK_INTERNAL
void framebuffer_size_callback(GLFWwindow* window, s32 width, s32 height)
{
    glViewport(0, 0, width, height);
}

GLFWframebuffersizefun Renderer::get_framebuffer_size_callback() const noexcept
{
    return framebuffer_size_callback;
}

std::string Renderer::get_gl_api_version() const noexcept
{
    return fmt::format("OpenGL {}.{} core", m_opengl_ctx_version_major, m_opengl_ctx_version_minor);
}
