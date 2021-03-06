#include "core/core.h"

#include "core/assert.h"
#include "core/logging/logging.h"
#include "core/platform/glfw.h"
#include "core/renderer/opengl/shader_program.h"
#include "core/utility/fixme.h"
#include "core/utility/no_exception.h"
#include "core/utility/stb_image.h"
#include <sds/array.h>
#include <sds/array/make_array.h>

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
    Shader_Program simple_shader("identity.vert", "identity.frag");
    RK_CHECK(simple_shader.compile());

    constexpr auto vertices =
        sds::make_array<f32>(-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f);

    u32 vao = 0;
    glGenVertexArrays(1, &vao);
    u32 vbo = 0;
    glGenBuffers(1, &vbo);

    { // Config vao
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sds::byte_size(vertices), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), nullptr);
        glEnableVertexAttribArray(0);

        // glVertexAttribPointer registered the vbo as the vertex attributes buffer. Can safely
        // unbind.
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    constexpr auto rectangle_vertices = sds::make_array<f32>(0.5f, 0.5f, 0.0f,   // top right
                                                             0.5f, -0.5f, 0.0f,  // bottom right
                                                             -0.5f, -0.5f, 0.0f, // bottom left
                                                             -0.5f, 0.5f, 0.0f   // top left
    );

    constexpr auto rectangle_indicies = sds::make_array<u32>(3U, 1U, 0U, // 1st
                                                             3U, 2U, 1U  // 2nd
    );

    u32 rectangle_vao = 0;
    u32 rectangle_vbo = 0;
    u32 rectangle_ebo = 0;
    glGenVertexArrays(1, &rectangle_vao);
    glGenBuffers(1, &rectangle_vbo);
    glGenBuffers(1, &rectangle_ebo);

    { // Config rectangle vao
        glBindVertexArray(rectangle_vao);
        glBindBuffer(GL_ARRAY_BUFFER, rectangle_vbo);
        glBufferData(GL_ARRAY_BUFFER, sds::byte_size(rectangle_vertices), rectangle_vertices.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangle_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sds::byte_size(rectangle_indicies),
                     rectangle_indicies.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), nullptr);
        glEnableVertexAttribArray(0);

        // glVertexAttribPointer registered the vbo as the vertex attributes buffer. Can safely
        // unbind.
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    glClearColor(0.4f, 0.4f, 0.7f, 1.0f);

    Window& window = m_window_mgr->get_window();

    bool running = true;
    while (!window.should_close_window() && running) {
        { // Input
            RK_CHECK(m_input_mgr->process_new_input());

            Game_Input const& input = m_input_mgr->get_input();
            Window_Settings const& window_settings = input.settings.window;
            Graphics_Settings const& graphics_settings = input.settings.graphics;

            running = !input.state.request_quit;
            if (!running) { break; }

            RK_CHECK(window.set_fullscreen(window_settings.fullscreen));
            m_renderer->draw_wireframe(graphics_settings.wireframe);
        }

        { // Rendering
            glClear(GL_COLOR_BUFFER_BIT);

            simple_shader.use();

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glBindVertexArray(rectangle_vao);
            glDrawElements(
                GL_TRIANGLES,
                fixme::scast<s32>(rectangle_indicies.size(), "replace std::array and avoid size_t"),
                GL_UNSIGNED_INT, nullptr);

            glBindVertexArray(0);
        }

        RK_CHECK(m_renderer->swap_buffers());
        Logger::flush(); // @perf: do this on an async thread
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &rectangle_vao);
    glDeleteBuffers(1, &rectangle_vbo);
    simple_shader.destroy(); // not explicitly necessary, but good practice

    return Status::ok;
}

bool Rtek_Engine::m_initialized = false;
