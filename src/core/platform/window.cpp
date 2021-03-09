#include "core/platform/window.h"

#include "GLFW/glfw3.h"
#include "core/logging/logging.h"

using namespace rk;

// void Window::window_resize_callback(GLFWwindow* glfw_window, s32 window_w, s32 window_h) noexcept
// {
//     Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
//     RK_ASSERT(window);
//     window->resize(window_w, window_h);
// }

// void Window::resize(s32 width, s32 height) noexcept
// {
//     // TODO(sdsmith): update the viewport
//     if (!is_fullscreen()) {
//         // Update the location of the non-fullscreen window
//         m_window_size[0] = width;
//         m_window_size[1] = height;
//     }
// }

Status Window::initialize(char const* title, s32 width, s32 height) noexcept
{
    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        LOG_CRITICAL("Failed to create window '{}'", title);
        platform::glfw::destroy();
        return Status::window_error;
    }

    // Set user data associated with window to `this`. Retrievable with `glfwGetWindowUserPointer`.
    glfwSetWindowUserPointer(m_window, this);

    // TODO(sdsmith): necessary?
    // glfwSetWindowSizeCallback(m_window, window_resize_callback);

    // TODO(sdsmith): only supports using the primary monitor. Support using the last monitor the
    // window was on for fullscreen/window toggle
    m_monitor = glfwGetPrimaryMonitor();

    // TODO(sdsmith): these aren't actually needed AFIAK because the m_window_{size,pos} are only
    // used to store the last non-fullscreen values so we can exit fullscreen and return to the old
    // layout.
    glfwGetWindowSize(m_window, &m_window_size[0], &m_window_size[1]);
    glfwGetWindowPos(m_window, &m_window_pos[0], &m_window_pos[1]);

    return Status::ok;
}

bool Window::is_initialized() const noexcept { return m_window != nullptr; }

bool Window::is_fullscreen() const noexcept { return glfwGetWindowMonitor(m_window) != nullptr; }

Status Window::set_fullscreen(bool enable) noexcept
{
    if (is_fullscreen() == enable) { return Status::ok; }

    if (enable) {
        // Backup window state for future transition out of fullscreen
        glfwGetWindowPos(m_window, &m_window_pos[0], &m_window_pos[1]);
        glfwGetWindowSize(m_window, &m_window_size[0], &m_window_size[1]);

        // Get monitor resolution
        GLFWvidmode const* vidmode = glfwGetVideoMode(m_monitor);
        RK_CHECK(platform::glfw::handle_error());
        RK_ASSERT(vidmode);

        // Switch to full screen
        glfwSetWindowMonitor(m_window, m_monitor, 0, 0, vidmode->width, vidmode->height,
                             vidmode->refreshRate /* or GLFW_DONT_CARE */);
    } else {
        glfwSetWindowMonitor(m_window, nullptr, m_window_pos[0], m_window_pos[1], m_window_size[0],
                             m_window_size[1], 0);
    }

    // TODO(sdsmith): renderer viewport needs updating. NOTE(sdsmith): may not actually need to do
    // this, as the framebuffer callback will handle changing the viewport if the framebuffer size
    // has changed.

    return platform::glfw::handle_error();
}

Status Window::is_current_context(bool& is_current) const noexcept
{
    is_current = m_window == glfwGetCurrentContext();
    return platform::glfw::handle_error();
}

Status Window::make_current_context() const noexcept
{
    glfwMakeContextCurrent(m_window);
    return platform::glfw::handle_error();
}

Status Window::set_framebuffer_size_callback(GLFWframebuffersizefun callback) const noexcept
{
    glfwSetFramebufferSizeCallback(m_window, callback);
    return platform::glfw::handle_error();
}

Status Window::set_key_callback(GLFWkeyfun callback) const noexcept
{
    glfwSetKeyCallback(m_window, callback);
    return platform::glfw::handle_error();
}

bool Window::should_close_window() const noexcept { return glfwWindowShouldClose(m_window); }

Status Window::get_window_size(s32& width, s32& height) const noexcept
{
    glfwGetWindowSize(m_window, &width, &height);
    return platform::glfw::handle_error();
}

Status Window::swap_buffers() const noexcept
{
    glfwSwapBuffers(m_window);
    return platform::glfw::handle_error();
}
