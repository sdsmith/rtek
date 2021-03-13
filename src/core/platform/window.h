#pragma once

#include "core/platform/glfw.h"
#include "core/status.h"
#include <array>

namespace rk
{
class Window {
public:
    Window() = default;

    [[nodiscard]] Status initialize(char const* title, s32 width, s32 height) noexcept;

    [[nodiscard]] bool is_initialized() const noexcept;

    [[nodiscard]] bool is_fullscreen() const noexcept;

    [[nodiscard]] Status set_fullscreen(bool enable) noexcept;

    [[nodiscard]] Status is_current_context(bool& is_current) const noexcept;

    [[nodiscard]] Status make_current_context() const noexcept;

    [[nodiscard]] Status set_framebuffer_size_callback(
        GLFWframebuffersizefun callback) const noexcept;

    [[nodiscard]] Status set_key_callback(GLFWkeyfun callback) const noexcept;

    [[nodiscard]] bool should_close_window() const noexcept;

    [[nodiscard]] Status get_window_size(s32& width, s32& height) const noexcept;

    [[nodiscard]] Status swap_buffers() const noexcept;

private:
    std::array<s32, 2> m_window_pos{0, 0};  // x, y
    std::array<s32, 2> m_window_size{0, 0}; // width, height

    GLFWwindow* m_window = nullptr;
    GLFWmonitor* m_monitor = nullptr;

    [[nodiscard]] Status set_window_icon() noexcept;
};
} // namespace rk
