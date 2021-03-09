#pragma once

#include "core/platform/glfw.h"
#include "core/platform/input_manager.h"
#include "core/platform/window.h"
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
    Status create_window(char const* title, s32 window_w, s32 window_h, Renderer& renderer,
                         Input_Manager& input_mgr);

    /**
     * \brief Get the current window.
     *
     * NOTE(sdsmith): Assumes there is only one window.
     */
    [[nodiscard]] Window& get_window() noexcept;

private:
    Window m_window;
};
} // namespace rk
