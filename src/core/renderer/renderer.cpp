#include "core/renderer/renderer.h"

#include "core/assert.h"
#include "core/logging/logging.h"
#include "core/platform/glfw.h"
#include "core/types.h"
#include "core/utility/fixme.h"
#include <fmt/core.h>
#include <glad/glad.h>
#include <sds/string.h>
#include <cstring>
#include <iterator>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/gtc/matrix_transform.hpp>

// Ask for a high performance renderer
#include "core/renderer/request_high_perf_renderer.h"

using namespace rk;
using namespace sds;

Global_Renderer_State rk::g_renderer_state = {};

Status Renderer::initialize(Config config) noexcept {
    m_config = config;
    return Status::ok;
}

Status Renderer::destroy() noexcept {
    glDeleteVertexArrays(1, &m_text_vao);
    glDeleteBuffers(1, &m_text_vbo);

    // TODO: cleanup glyphs?

    return Status::ok;
}

Status Renderer::prepare_window() noexcept
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_ogl_ctx_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_ogl_ctx_version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,
        (m_config.enable_debug ? GLFW_TRUE : GLFW_FALSE));

    return platform::glfw::handle_error();
}

void Renderer::set_window(Window& window) noexcept { m_window = &window; }

/**
 * \brief Convert an OpenGL error to a string.
 */
char const* to_string_ogl_error(GLenum ogl_error) noexcept
{
    switch (ogl_error) {
        case GL_NO_ERROR: return "NO_ERROR";
        case GL_INVALID_ENUM: return "INVALID_ENUM";
        case GL_INVALID_VALUE: return "INVALID_VALUE";
        case GL_INVALID_OPERATION: return "INVALID_OPERATION";
        case GL_STACK_OVERFLOW: return "STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW: return "STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY: return "OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID_FRAMEBUFFER_OP";
        case GL_CONTEXT_LOST: return "CONTEXT_LOST";
    }

    LOG_CRITICAL("Unknown OpenGL error: {}", ogl_error);
    RK_ASSERT(0);
    return "unknown";
}

/**
 * \brief Handle an OpenGL error. Return ok if no error.
 */
Status Renderer::handle_ogl_error() const noexcept
{
    GLenum err = GL_NO_ERROR;
    bool was_err = false;
    while ((err = glGetError()) != GL_NO_ERROR) {
        LOG_ERROR("OpenGL error: {}", to_string_ogl_error(err));
        was_err = true;
    }

    if (was_err) { return Status::renderer_error; }

    return Status::ok;
}

/**
 * \brief Convert OpenGL \A KHR_debug type to string.
 */
RK_INTERNAL
char const* to_string_ogl_debug_type(GLenum type) noexcept
{
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: return "error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "deprecated behaviour";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UB";
        case GL_DEBUG_TYPE_PORTABILITY: return "portability";
        case GL_DEBUG_TYPE_PERFORMANCE: return "performance";
        case GL_DEBUG_TYPE_OTHER: return "other";
    }

    LOG_CRITICAL("Unknown OpenGL debug type: {}", type);
    RK_ASSERT(0);
    return "unknown";
}

/**
 * \brief Convert OpenGL \A KHR_debug severity to string.
 */
RK_INTERNAL
char const* to_string_ogl_debug_severity(GLenum severity) noexcept
{
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: return "high";
        case GL_DEBUG_SEVERITY_MEDIUM: return "medium";
        case GL_DEBUG_SEVERITY_LOW: return "low";
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "notification";
    }

    LOG_CRITICAL("Unknown OpenGL severity type: {}", severity);
    RK_ASSERT(0);
    return "unknown";
}

/**
 * \brief Convert OpenGL \A KHR_debug source to string.
 */
RK_INTERNAL
char const* to_string_ogl_debug_source(GLenum source) noexcept
{
    switch (source) {
        case GL_DEBUG_SOURCE_API: return "api";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "window system";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "shader compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "3rd party";
        case GL_DEBUG_SOURCE_APPLICATION: return "application";
        case GL_DEBUG_SOURCE_OTHER: return "other";
    }

    LOG_CRITICAL("Unknown OpenGL source type: {}", source);
    RK_ASSERT(0);
    return "unknown";
}

/**
 * \brief Callback to process OpenGL \a KHR_debug messages.
 *
 * \see glDebugMessageCallback
 */
RK_INTERNAL
void GLAPIENTRY ogl_debug_callback(GLenum source, GLenum type, u32 id, GLenum severity, s32 length,
                                   char const* message, void const* user_param) noexcept
{
    RK_ASSERT(message);
    RK_ASSERT(!user_param); // warn when user param is set

    // @cleanup: adjust log level of message based on severity based on a param
    // (requires new LOG(level, ...) macro

    if (type == GL_DEBUG_TYPE_ERROR) {
        LOG_ERROR("[OpenGL] type: {}, source: {}, id: {}, severity: {}, message: {}",
                  to_string_ogl_debug_type(type), to_string_ogl_debug_source(source), id,
                  to_string_ogl_debug_severity(severity), message);



        // TODO(sdsmith): Fail?
        RK_ASSERT(!"OGL error");

    } else {
        LOG_DEBUG("[OpenGL] type: {}, source: {}, id: {}, severity: {}, message: {}",
                  to_string_ogl_debug_type(type), to_string_ogl_debug_source(source), id,
                  to_string_ogl_debug_severity(severity), message);
    }
}

// TODO(sdsmith): this feels very platform-y
Status Renderer::setup_gl_api() noexcept
{
    // Renderer's window must be the current rendering context.
    bool is_current_context = false;
    RK_CHECK(m_window->is_current_context(is_current_context));
    RK_ASSERT(is_current_context);

    // Initialize GLAD with the OS-specific OpenGL function pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        LOG_CRITICAL("Failed to initialize GLAD");
        return Status::renderer_error;
    }

    // Print renderer information
    LOG_INFO(
        "Renderer info:\n"
        "  Vendor       : {}\n"
        "  Renderer     : {}\n"
        "  Graphics API : OpenGL {}",
        glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

    // Check that we are in a debug context
    s32 ctx_flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &ctx_flags);
    bool const is_ogl_debug_ctx = (ctx_flags & GL_CONTEXT_FLAG_DEBUG_BIT);

    if (m_config.enable_debug) {
        RK_ASSERT(is_ogl_debug_ctx);

        // NOTE(sdsmith): @perf: Enabling synchronous debug callbacks is slow, but provides
        // better debugging. By default is is async, which has no guarantees that it
        // is called on the same thread or at the same time that the message was
        // created. Async is faster.
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(ogl_debug_callback, nullptr /*user_param*/);

        // Enable all messages
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        char const* khr_debug_msg = "Setup KHR_debug message callback";
        glDebugMessageInsert(
            GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, -1, GL_DEBUG_SEVERITY_NOTIFICATION,
            fixme::scast<s32>(std::strlen(khr_debug_msg), "replace strlen"), khr_debug_msg);
        RK_CHECK(handle_ogl_error());
    }

    // Explicitly set counter-clockwise winding order
    glFrontFace(GL_CCW);

    // Enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    LOG_INFO(
        "OpenGL context info:\n"
        "  Debug: {}",
        sds::to_string(is_ogl_debug_ctx));

    s32 window_w = 0;
    s32 window_h = 0;
    RK_CHECK(m_window->get_window_size(window_w, window_h));
    glViewport(0, 0, window_w, window_h);

    LOG_INFO("OpenGL context initialized");
    return Status::ok;
}

void Renderer::draw_wireframe(bool enable) const noexcept
{
    if (enable) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

Status Renderer::swap_buffers() const noexcept { return m_window->swap_buffers(); }

/**
 * \brief Window resize callback function.
 */
RK_INTERNAL
void framebuffer_size_callback(GLFWwindow* window, s32 width, s32 height) noexcept
{
    RK_ASSERT(window);
    LOG_INFO("Window resized to width: {}, height: {} - adjusting viewport", width, height);
    glViewport(0, 0, width, height);

    // Update the ortho_projection
    g_renderer_state.screen_ortho_projection = glm::ortho(0.0f, static_cast<f32>(width), 0.0f, static_cast<f32>(height));
}

GLFWframebuffersizefun Renderer::get_framebuffer_size_callback() const noexcept
{
    return framebuffer_size_callback;
}

Status Renderer::load_font_glyphs() noexcept {
    // TODO: update to take font param. Will need a charcode map per active font
#define RK_FT_SAME_AS_OTHER_DIM 0
    FT_Library ft;
    FT_Error err = FT_Init_FreeType(&ft);
    if (err) {
        LOG_ERROR("Failed to initialize FreeType: {}", FT_Error_String(err));
        return Status::api_error;
    }

    FT_Face face;
    // TODO: update to rk::filesystem path API
    std::string font_path = fmt::format("{}/{}", RK_DATA_BASE_DIR, "assets/fonts/calibri/calibri-regular.ttf");
    err = FT_New_Face(ft, font_path.c_str(), 0, &face);
    if (err) {
        LOG_ERROR("Failed to load font face ({}): {}", font_path.c_str(), FT_Error_String(err));
        return Status::io_error;
    }

    // Set charmap encoding
    const FT_Encoding encoding = FT_ENCODING_UNICODE;
    err = FT_Select_Charmap(face, encoding);
    if (err) {
        LOG_ERROR("Failed to select encoding for '{}': {}", font_path, FT_Error_String(err));
        return Status::api_error;
    }

    // NOTE: char heights and widths are specified in 1/64 of a point. A point is a physical distance equal to 1/72 of an inch.

#if 0
    err = FT_Set_Char_Size(
        face,
        RK_FT_SAME_AS_OTHER_DIM, // char_width in 1/64 of points
        16*64, // char_height in 1/64 of points
        window_w, // horizontal device resolution
        window_h // vertical device resolution
    );
    if (err) {
        LOG_ERROR("Failed to set font glyph character size for '{}': {}", font_path, FT_Error_String(err));
        return Status::api_error;
    }
#else
    err = FT_Set_Pixel_Sizes(
        face,
        RK_FT_SAME_AS_OTHER_DIM, // pixel width
        64 // pixel height
    );
    if (err) {
        LOG_ERROR("Failed to set font glyph pixel size for '{}': {}", font_path, FT_Error_String(err));
        return Status::api_error;
    }
#endif
#undef RK_FT_SAME_AS_OTHER_DIM

    // Cache the 128 ASCII characters
    //
    // NOTE: The glyph is a grayscale 8-bit image with a single byte per color. Disable OGL 32-bit alignment requirement so we can maintain one byte per color.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (u32 charcode = 0; charcode < 128; ++charcode) {
        err = FT_Load_Char(face, charcode, FT_LOAD_RENDER);
        if (err) {
            LOG_ERROR("Failed to load glyph for character code '{}'", charcode);
            continue;
        }

        u32 texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character c{
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        m_characters[charcode] = c;
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Create text quad buffer
    glGenVertexArrays(1, &m_text_vao);
    glGenBuffers(1, &m_text_vbo);
    glBindVertexArray(m_text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_text_vbo);
    // 6 verts in quad, 4 floats each
    // dynamic draw - will be changing buffer content often
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return Status::ok;
}

Status Renderer::render_text(Shader_Program& shader, std::string_view text, f32 screen_pos_x, f32 screen_pos_y, f32 scale, glm::vec3 color) const noexcept {
    RK_ASSERT(!m_characters.empty());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader.use();
    shader.set_vec3("text_color", color);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_text_vao);

    for (char c : text) {
        // TODO: can we access this by ref?
        auto ch_iter = m_characters.find(c);
        if (ch_iter == m_characters.end()) {
            // TODO: error - unable to find glyph for character
            continue;
        }
        Character ch = ch_iter->second;

        f32 pos_x = screen_pos_x + ch.bearing.x * scale;
        f32 pos_y = screen_pos_y - (ch.size.y - ch.bearing.y) * scale;

        f32 w = ch.size.x * scale;
        f32 h = ch.size.y * scale;

        // update vbo per character to create the rect it will be rendered in
        // @perf don't allocate this every time (will it get hoisted?)
        f32 vertices[6][4] = {
            { pos_x,     pos_y + h, 0.0f, 0.0f },
            { pos_x,     pos_y,     0.0f, 1.0f },
            { pos_x + w, pos_y,     1.0f, 1.0f },

            { pos_x,     pos_y + h, 0.0f, 0.0f },
            { pos_x + w, pos_y,     1.0f, 1.0f },
            { pos_x + w, pos_y + h, 1.0f, 0.0f },
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.texture_id);
        // update vbo
        glBindBuffer(GL_ARRAY_BUFFER, m_text_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // advance cursor for next glyph
        // NOTE: advance is number of 1/64 points
        //  bitshift >> 6 => 2^6 = 64
        screen_pos_x += (ch.advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);

    return Status::ok;
}
