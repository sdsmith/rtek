#include "core/core.h"

#include "core/logging/logging.h"
#include "core/platform/glfw.h"
#include "core/utility/assert.h"
#include "core/utility/no_exception.h"

using namespace rk;

Status Rtek_Engine::initialize() noexcept
{
    RK_CHECK(Logger::initialize());
    LOG_INFO("Logger initialized");
    LOG_INFO("Initializing engine...");

    LOG_INFO("Initializing GLFW...");
    RK_CHECK(platform::glfw::initialize());

    LOG_INFO("Initializing the window manager...");
    RK_CHECK_EXB(exception_boundary([&]() {
        m_window_mgr = std::make_unique<Window_Manager>();
        return Status::ok;
    }));
    RK_CHECK(m_window_mgr->initialize());

    LOG_INFO("Initializing the input manager...");
    RK_CHECK_EXB(exception_boundary([&]() {
        m_input_mgr = std::make_unique<Input_Manager>();
        return Status::ok;
    }));
    RK_CHECK(m_input_mgr->initialize());

    LOG_INFO("Initializing the renderer...");
    RK_CHECK_EXB(exception_boundary([&]() {
        m_renderer = std::make_unique<Renderer>();
        return Status::ok;
    }));
    RK_CHECK(m_renderer->initialize());

    LOG_INFO("Initializing rendering context...");
    constexpr s32 window_w = 800;
    constexpr s32 window_h = 600;
    RK_CHECK(m_window_mgr->create_window("RTek", window_w, window_h, *m_renderer, *m_input_mgr));
    RK_CHECK(m_renderer->setup_gl_api());

    LOG_INFO("Engine initialized");
    m_initialized = true;
    return Status::ok;
}

Status Rtek_Engine::destroy() noexcept
{
    if (!m_initialized) {
        LOG_ERROR("Attempt to destroy uninitialized engine");
        return Status::api_error;
    }

    m_window_mgr->destroy();
    m_input_mgr->destroy();
    m_renderer->destroy();
    platform::glfw::destroy();

    m_initialized = false;
    LOG_INFO("Engine destroyed");
    Logger::flush();
    return Status::ok;
}

Status Rtek_Engine::run() noexcept
{
    bool running = true;
    while (!m_window_mgr->should_close_window() && running) {
        RK_CHECK(m_input_mgr->process_new_input());

        Game_Input const& input = m_input_mgr->get_input();
        running = !input.state.request_quit;
        if (!running) { break; }

        glClearColor(0.4f, 0.4f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        RK_CHECK(m_renderer->swap_buffers());
    }

    return Status::ok;
}

bool Rtek_Engine::m_initialized = false;
