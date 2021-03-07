#include "core/core.h"

#include "core/logging/logging.h"
#include "core/platform/glfw.h"
#include "core/utility/assert.h"
#include "core/utility/no_exception.h"
#include <sdslib/array/make_array.h>

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
    u32 shader_program = glCreateProgram();
    { // Create shader program
        u32 vert_shader = glCreateShader(GL_VERTEX_SHADER);
        RK_CHECK(m_renderer->compile_shader("identity.vert", vert_shader));

        u32 frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
        RK_CHECK(m_renderer->compile_shader("identity.frag", frag_shader));

        glAttachShader(shader_program, vert_shader);
        glAttachShader(shader_program, frag_shader);
        glLinkProgram(shader_program);

        s32 link_success = 0;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &link_success);
        if (!link_success) {
            LOG_ERROR("Failed to link program {}: {}", shader_program,
                      m_renderer->get_program_info_log(shader_program));
        }

        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);
    }

    constexpr auto vertices =
        sds::make_array<f32>(-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f);

    u32 vao = 0;
    glGenVertexArrays(1, &vao);
    u32 vbo = 0;
    glGenBuffers(1, &vbo);

    { // Config vao
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(f32), vertices.data(),
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0); // unbind
    }

    bool running = true;
    while (!m_window_mgr->should_close_window() && running) {
        RK_CHECK(m_input_mgr->process_new_input());

        Game_Input const& input = m_input_mgr->get_input();
        running = !input.state.request_quit;
        if (!running) { break; }

        glClearColor(0.4f, 0.4f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Prepare to draw
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        RK_CHECK(m_renderer->handle_ogl_error());

        RK_CHECK(m_renderer->swap_buffers());
        Logger::flush(); // @perf: do this on an async thread
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_program);

    return Status::ok;
}

bool Rtek_Engine::m_initialized = false;
