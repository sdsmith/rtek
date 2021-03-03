#pragma once

#include "core/platform/glfw.h"
#include "core/platform/input_manager.h"
#include "core/renderer/renderer.h"
#include "core/utility/status.h"

namespace rk
{
class Window_Manager {
public:
    [[nodiscard]] Status initialize() noexcept;
    Status destroy() noexcept;

    /**
     * \brief Create a window and rendering context.
     */
    Status create_window(const char* title, s32 window_w, s32 window_h, Renderer& renderer,
                         Input_Manager& input_mgr);

    [[nodiscard]] bool should_close_window() const noexcept;

    [[nodiscard]] GLFWwindow* get_window() const noexcept;

private:
    GLFWwindow* m_window = nullptr;
};
} // namespace rk
