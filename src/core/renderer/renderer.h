#pragma once

#include "core/platform/glfw.h"
#include "core/utility/status.h"
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
     * \see Renderer::setup_gl_api
     */
    [[nodiscard]] Status prepare_window() noexcept;

    /**
     * \brief Set the window with the render context.
     */
    void set_window(GLFWwindow* window) noexcept;

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
     * \brief Swap frame buffers.
     */
    [[nodiscard]] Status swap_buffers() const noexcept;

    [[nodiscard]] GLFWframebuffersizefun get_framebuffer_size_callback() const noexcept;

    [[nodiscard]] std::string get_gl_api_version() const noexcept;

    [[nodiscard]] std::string get_program_info_log(u32 id) const noexcept;

    /**
     * Get the content of a shader file.
     */
    [[nodiscard]] Status compile_shader(char const* name, u32 shader_id) const noexcept;

private:
    GLFWwindow* m_window = nullptr;
    static constexpr s32 m_ogl_ctx_version_major = 4;
    static constexpr s32 m_ogl_ctx_version_minor = 6;
};
} // namespace rk
