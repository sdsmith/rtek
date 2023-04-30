#include "core/core.h"

#include "core/assert.h"
#include "core/logging/logging.h"
#include "core/platform/glfw.h"
#include "core/renderer/opengl/shader_program.h"
#include "core/utility/fixme.h"
#include "core/utility/no_exception.h"
#include "core/utility/stb_image.h"
#include <sds/array/array.h>
#include <sds/array/make_array.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

using namespace rk;
using namespace sds;


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
    Renderer::Config renderer_config = {};
#ifdef RK_OGL_DEBUG
    // TODO: set based on CLI args
    renderer_config.enable_debug = true;
#endif
    RK_CHECK(m_renderer->initialize(renderer_config));

    LOG_INFO("Initializing rendering context...");
    constexpr s32 window_w = 800;
    constexpr s32 window_h = 600;
    RK_CHECK(m_window_mgr->create_window("RTek", window_w, window_h, *m_renderer, *m_input_mgr));

    RK_CHECK(m_renderer->setup_gl_api());
    RK_CHECK(m_renderer->load_font_glyphs());

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

    Shader_Program text_shader("font_glyphs.vert", "font_glyphs.frag");
    {
        RK_CHECK(text_shader.compile());

        // ortho projection (text always renders in screen space... for now)
        // Setup matrix so that vertex coodinates are 1:1 with screen space
        s32 win_w = 0;
        s32 win_h = 0;
        RK_CHECK(m_window_mgr->get_window().get_window_size(win_w, win_h));
        g_renderer_state.screen_ortho_projection = glm::ortho(0.0f, static_cast<f32>(win_w), 0.0f, static_cast<f32>(win_h));
        text_shader.use();
        glUniformMatrix4fv(glGetUniformLocation(text_shader.handle(), "projection"), 1, GL_FALSE, glm::value_ptr(g_renderer_state.screen_ortho_projection));
    }

    constexpr auto vertices =
        sds::make_array<f32>(-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f);

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

            glBindVertexArray(rectangle_vao);
            glDrawElements(
                GL_TRIANGLES,
                fixme::scast<s32>(rectangle_indicies.size(), "replace std::array and avoid size_t"),
                GL_UNSIGNED_INT, nullptr);

            glBindVertexArray(0);
        }

        { // Overlay
            RK_CHECK(m_renderer->render_text(text_shader, "Hello world!", 25.0f, 25.0f, 1.0f, glm::vec3(.5f, .8f, .2f)));
            RK_CHECK(m_renderer->render_text(text_shader, "!@#$%^&*()_+", 575.0f, 560.0f, .5f, glm::vec3(.5f, .8f, .2f)));
        }

        RK_CHECK(m_renderer->swap_buffers());
        Logger::flush(); // @perf: do this on an async thread
    }

    glDeleteVertexArrays(1, &rectangle_vao);
    glDeleteBuffers(1, &rectangle_vbo);
    // not explicitly necessary to destory these shaders, but good practice
    simple_shader.destroy();
    text_shader.destroy();

    return Status::ok;
}

bool Rtek_Engine::m_initialized = false;
