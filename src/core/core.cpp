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

#include <ft2build.h>
#include FT_FREETYPE_H

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
    RK_CHECK(m_renderer->initialize());

    LOG_INFO("Initializing rendering context...");
    constexpr s32 window_w = 800;
    constexpr s32 window_h = 600;
    RK_CHECK(m_window_mgr->create_window("RTek", window_w, window_h, *m_renderer, *m_input_mgr));
    RK_CHECK(m_renderer->setup_gl_api());

    {
#define RK_FT_SAME_AS_OTHER_DIM 0
        FT_Library ft_lib;
        FT_Error err = FT_Init_FreeType(&ft_lib);
        if (err) {
            // TODO:
        }

        FT_Face font_face;
        err = FT_New_Face(ft_lib, fmt::format("{}/{}", RK_DATA_BASE_DIR, "fonts/calibri/calibri-regular.ttf").c_str(), 0, &font_face);
        if (err == FT_Err_Unknown_File_Format) {
            // TODO: file opened and read, but has unsupporte format
        } else if (err) {
            // TODO: file could not be open or read
        }

        // Set charmap encoding
        const FT_Encoding encoding = FT_ENCODING_UNICODE;
        err = FT_Select_Charmap(font_face, encoding);

        // NOTE: char heights and widths are specified in 1/64 of a point. A point is a physical distance equal to 1/72 of an inch.

#if 0
        err = FT_Set_Char_Size(
            font_face,
            RK_FT_SAME_AS_OTHER_DIM, // char_width in 1/64 of points
            16*64, // char_height in 1/64 of points
            window_w, // horizontal device resolution
            window_h // vertical device resolution
        );
        if (err) {
            // TODO:
        }
#else
        err = FT_Set_Pixel_Sizes(
            font_face,
            RK_FT_SAME_AS_OTHER_DIM, // pixel width
            16 // pixel height
        );
        if (err) {
            // TODO:
        }
#endif

        FT_GlyphSlot slot = font_face->glyph;
        std::string s = "hello world";
        s32 pen_x = window_w;
        s32 pen_y = window_h;
        for (char c : s) {
            // Load glyph into slot
            // @perf may be faster to FT_Get_Char_Index, FT_Load_Glyph, then cache the result of FT_Render_Glyph
            err = FT_Load_Glyph(font_face, c, FT_LOAD_RENDER);
            if (err) {
                // TODO:
            }

            // draw to target surface
            // NOTE: For ideal rendering on a screen this function should perform linear blending with gamma correction, using the bitmap as an alpha channel.
            // NOTE: Note that bitmap_left is the horizontal distance from the current pen position to the leftmost border of the glyph bitmap, while bitmap_top is the vertical distance from the pen position (on the baseline) to the topmost border of the glyph bitmap. It is positive to indicate an upwards distance.
/*             draw_bitmap(&slot->bitmap,
                        pen_x + slot->bitmap_left,
                        pen_y - slot->bitmap_top // assumes increasing Y corresponds to downward scanlines (hence the subtraction)
                        ); */

            // increment pen
            pen_x += slot->advance.x >> 6;
        }

        // TODO: "More Advanced Rendering" https://freetype.org/freetype2/docs/tutorial/step1.html

#undef SAME_AS_OTHER_DIM
    }

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
