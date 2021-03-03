#pragma once

#include "core/hid/input.h"
#include "core/platform/glfw.h"
#include "core/utility/status.h"

namespace rk
{
class Input_Manager {
public:
    Input_Manager() = default;

    Status initialize() noexcept;
    Status destroy() noexcept;

    [[nodiscard]] GLFWkeyfun get_keyboard_event_callback() const noexcept;

    /**
     * \brief Get latest input.
     */
    [[nodiscard]] Game_Input& get_input() noexcept;

    /**
     * Process all pending input events.
     */
    Status process_new_input() noexcept;

    /** Input for the current cycle. Populated by the keyboard input callback. */
    static Game_Input buffer_input;

private:
    std::array<Game_Input, 2> m_input;
    Game_Input* m_new_input = &m_input[0];
    Game_Input* m_old_input = &m_input[1];

    static constexpr Game_Input zeroed_input = {};

    /**
     * \brief Prepare for the new input from a new cycle.
     */
    void prepare_for_new_input() noexcept;
};
} // namespace rk
