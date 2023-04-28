#include "core/platform/window_manager.h"

#include "GLFW/glfw3.h"
#include "core/logging/logging.h"
#include "core/platform/glfw.h"

using namespace rk;
using namespace sds;

Status Window_Manager::initialize() noexcept { return Status::ok; }

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
