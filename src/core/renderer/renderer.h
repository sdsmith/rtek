#pragma once

#include "core/platform/glfw.h"
#include "core/platform/window.h"
#include "core/renderer/opengl/shader_program.h"
#include "core/status.h"
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <map>
#include <string>

namespace rk
{
class Renderer {
public:
    Renderer() = default;

    [[nodiscard]] Status initialize() noexcept;
    Status destroy() noexcept;

    /**
     * \brief Prepare for a window creation.
     *
     * The type of rendering being done requires a specific type of window context.
     *
     * \see Window_Manager::create_window
     * \see Renderer::setup_gl_api
     */
    [[nodiscard]] Status prepare_window() noexcept;

    /**
     * \brief Set the window with the render context.
     */
    void set_window(Window& window) noexcept;

    /**
     * \brief Handle an OpenGL error. Return ok if no error.
     */
    [[nodiscard]] Status handle_ogl_error() const noexcept;

    /**
     * \brief Set up the environment for the graphics library API.
     *
     * Must be called before rendering can take place.
     */
    [[nodiscard]] Status setup_gl_api() noexcept;

    /**
     * \brief Draw wireframe primitives in any subsequent draw calls.
     */
    void draw_wireframe(bool enable) const noexcept;

    /**
     * \brief Swap frame buffers.
     */
    [[nodiscard]] Status swap_buffers() const noexcept;

    [[nodiscard]] GLFWframebuffersizefun get_framebuffer_size_callback() const noexcept;

    [[nodiscard]] Status load_font_glyphs() noexcept;
    [[nodiscard]] Status render_text(Shader_Program& shader, std::string_view text, f32 screen_pos_x, f32 screen_pos_y, f32 scale, glm::vec3 color) const noexcept;

private:
    Window* m_window = nullptr;
    static constexpr s32 m_ogl_ctx_version_major = 4;
    static constexpr s32 m_ogl_ctx_version_minor = 6;

    struct Character {
        u32 texture_id; // handle of glyph texture
        glm::ivec2 size; // glyph size
        glm::ivec2 bearing; // offset from baseline to left/top of glyph
        s64 advance; // offset to advance to next glyph
    };
    std::map<u32, Character> m_characters;

    u32 m_text_vao = 0;
    u32 m_text_vbo = 0;
    glm::mat4 m_screen_ortho_projection;
};
} // namespace rk
