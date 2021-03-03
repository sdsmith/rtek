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
     */
    [[nodiscard]] Status prepare_window() noexcept;

    /**
     * \brief Set the window with the render context.
     */
    void set_window(GLFWwindow* window) noexcept;

    /**
     * DOC(sdsmith):
     */
    [[nodiscard]] Status setup_gl_api();

    [[nodiscard]] Status swap_buffers() const noexcept;

    [[nodiscard]] GLFWframebuffersizefun get_framebuffer_size_callback() const noexcept;

    [[nodiscard]] std::string get_gl_api_version() const noexcept;

private:
    GLFWwindow* m_window = nullptr;
    static constexpr s32 m_opengl_ctx_version_major = 4;
    static constexpr s32 m_opengl_ctx_version_minor = 6;
};
} // namespace rk
