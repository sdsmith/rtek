#include "core/platform/window_manager.h"

#include "GLFW/glfw3.h"
#include "core/logging/logging.h"
#include "core/platform/glfw.h"
#include "core/types.h"

using namespace rk;

/**
 * \brief Called when the monitor configuration has changed.
 */
RK_INTERNAL
void monitor_configuration_callback(GLFWmonitor* monitor, s32 event)
{
    RK_ASSERT(monitor);

    // event: GLFW_CONNECTED, GLFW_DISCONNECTED

    Window* window = static_cast<Window*>(glfwGetMonitorUserPointer(monitor));

    switch (event) {
        case GLFW_CONNECTED:
            LOG_INFO("Monitor '{}' connected", glfwGetMonitorName(monitor));
            // TODO(sdsmith): check if there is a new primary and if we should change monitors.
            break;

        case GLFW_DISCONNECTED: {
            LOG_INFO("Monitor '{}' disconnected", glfwGetMonitorName(monitor));
            if (window) {
                // TODO(sdsmith): A window we use has been disconnected
            }
        } break;

        default:
            LOG_CRITICAL("Unknown GLFW monitor action on monitor '': {}",
                         glfwGetMonitorName(monitor), event);
            RK_ASSERT(0);
    }
}

Status Window_Manager::initialize() noexcept
{
    glfwSetMonitorCallback(monitor_configuration_callback);

    return platform::glfw::handle_error();
}

Status Window_Manager::destroy() noexcept { return Status::ok; }

Status Window_Manager::create_window(char const* title, s32 window_w, s32 window_h,
                                     Renderer& renderer, Input_Manager& input_mgr)
{
    RK_ASSERT(title);
    RK_ASSERT(window_w > 0 && window_h > 0);

    // Ask the renderer to hint at the type of window it needs prior to window creation
    RK_CHECK(renderer.prepare_window());

    // Create a window
    RK_CHECK(m_window.initialize(title, window_w, window_h));
    RK_CHECK(m_window.make_current_context());
    RK_CHECK(m_window.set_framebuffer_size_callback(renderer.get_framebuffer_size_callback()));
    RK_CHECK(m_window.set_key_callback(input_mgr.get_keyboard_event_callback()));

    // Inform renderer of window
    renderer.set_window(m_window);

    return Status::ok;
}

Window& Window_Manager::get_window() noexcept
{
    // TODO(sdsmith): initialization check
    return m_window;
}
