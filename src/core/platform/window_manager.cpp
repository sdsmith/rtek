#include "core/platform/window_manager.h"

#include "GLFW/glfw3.h"
#include "core/logging/logging.h"
#include "core/platform/glfw.h"

using namespace rk;

Status Window_Manager::initialize() noexcept { return Status::ok; }

Status Window_Manager::destroy() noexcept { return Status::ok; }

Status Window_Manager::create_window(const char* title, s32 window_w, s32 window_h,
                                     Renderer& renderer, Input_Manager& input_mgr)
{
    RK_ASSERT(title);
    RK_ASSERT(window_w > 0 && window_h > 0);

    // Ask the renderer to hint at the type of window it needs
    RK_CHECK(renderer.prepare_window());

    m_window = glfwCreateWindow(window_w, window_h, title, nullptr, nullptr);
    if (!m_window) {
        LOG_CRITICAL("Failed to create window");
        platform::glfw::destroy();
        return Status::window_error;
    }
    renderer.set_window(m_window);

    RK_CHECK_GLFW(glfwMakeContextCurrent(m_window));
    RK_CHECK_GLFW(
        glfwSetFramebufferSizeCallback(m_window, renderer.get_framebuffer_size_callback()));
    glfwSetKeyCallback(m_window, input_mgr.get_keyboard_event_callback());
    return platform::glfw::handle_error();
}

bool Window_Manager::should_close_window() const noexcept
{
    return glfwWindowShouldClose(m_window);
}

GLFWwindow* Window_Manager::get_window() const noexcept { return m_window; }
