#include "core/platform/window.h"

#include "GLFW/glfw3.h"
#include "core/logging/logging.h"
#include "core/utility/fixme.h"
#include "core/utility/stb_image.h"
#include <array>

using namespace rk;

/**
 * \brief Load image at the given path intended as a window icon into the given GLFW image
 * struct.
 *
 * \param path Path relative to \a RK_DATA_BASE_DIR.
 */
RK_INTERNAL
Status load_window_icon(char const* path, GLFWimage& img) noexcept
{
    RK_ASSERT(path);

    s32 num_channels = 0;
    img.pixels = stbi_load(fmt::format("{}/{}", RK_DATA_BASE_DIR, path).c_str(), &img.width,
                           &img.height, &num_channels, 0);
    if (!img.pixels) {
        LOG_ERROR(stbi_failure_reason());
        return Status::io_error;
    }

    RK_ASSERT(num_channels == 4); // glfwSetWindowIcon requires RGBA (4 channel) image format
    return Status::ok;
}

Status Window::set_window_icon() noexcept
{
    RK_ASSERT(m_window);

    std::array<GLFWimage, 4> icons;

    RK_CHECK(load_window_icon("assets/icons/rtek_16.png", icons[0]));
    RK_CHECK(load_window_icon("assets/icons/rtek_32.png", icons[1]));
    RK_CHECK(load_window_icon("assets/icons/rtek_48.png", icons[2]));
    RK_CHECK(load_window_icon("assets/icons/rtek_256.png", icons[3]));

    glfwSetWindowIcon(m_window,
                      fixme::scast<s32>(icons.size(), "replace std::array and avoid size_t"),
                      icons.data());
    RK_CHECK(platform::glfw::handle_error());

    // NOTE(sdsmith): GLFW copies the images, so they can be deleted
    for (GLFWimage& img : icons) { stbi_image_free(img.pixels); }

    return Status::ok;
}

Status Window::initialize(char const* title, s32 width, s32 height) noexcept
{
    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        LOG_CRITICAL("Failed to create window '{}'", title);
        platform::glfw::destroy();
        return Status::window_error;
    }

    RK_CHECK(set_window_icon());

    // Set user data associated with window to `this`. Retrievable with `glfwGetWindowUserPointer`.
    glfwSetWindowUserPointer(m_window, this);

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
