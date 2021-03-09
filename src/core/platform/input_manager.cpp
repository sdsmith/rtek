#include "core/platform/input_manager.h"

#include "core/logging/logging.h"
#include "core/types.h"
#include "core/utility/status.h"

using namespace rk;

Game_Input Input_Manager::input_buffer = Input_Manager::zeroed_input;

RK_INTERNAL
constexpr bool is_shift_key_active(s32 modifier_keys) noexcept
{
    return modifier_keys & GLFW_MOD_SHIFT;
}

RK_INTERNAL
constexpr bool is_ctrl_key_active(s32 modifier_keys) noexcept
{
    return modifier_keys & GLFW_MOD_CONTROL;
}

RK_INTERNAL
constexpr bool is_alt_key_active(s32 modifier_keys) noexcept
{
    return modifier_keys & GLFW_MOD_ALT;
}

RK_INTERNAL
constexpr bool is_super_key_active(s32 modifier_keys) noexcept
{
    return modifier_keys & GLFW_MOD_SUPER;
}

RK_INTERNAL
constexpr bool is_capslock_key_active(s32 modifier_keys)
{
    return modifier_keys & GLFW_MOD_CAPS_LOCK;
}

RK_INTERNAL
constexpr bool is_numlock_key_active(s32 modifier_keys)
{
    return modifier_keys & GLFW_MOD_NUM_LOCK;
}

Status Input_Manager::initialize() noexcept { return Status::ok; }

Status Input_Manager::destroy() noexcept { return Status::ok; }

/**
 * \brief Update button with state change.
 */
RK_INTERNAL
void update_input_button(Game_Input_Button& button, bool button_down) noexcept
{
    button.ended_down = button_down;
    button.half_transitions++;
}

RK_INTERNAL
char const* to_string_glfw_action(s32 action)
{
    switch (action) {
        case GLFW_PRESS: return "pressed";
        case GLFW_RELEASE: return "released";
        case GLFW_REPEAT: return "repeat";
    }

    LOG_ERROR("Unknown GLFW key action: {}", action);
    RK_ASSERT(0);
    return "unknown";
}

/**
 * \brief Process physical keyboard keys.
 *
 * \param window Window that recieved the event.
 * \param key Keyboard key that was pressed/released. Set to \a GLFW_KEY_UNKNOWN when a platform
 * specific key is pressed. See \a scancode in this case.
 * \param scancode System-specific scancode of the key.
 * \param action Type of key event.
 * \param mods Bit field describing active modifier keys.
 */
RK_INTERNAL
void process_keyboard_event_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action,
                                     s32 mods)
{
    RK_ASSERT(window);

    // action: GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT

    // @perf:
    char const* name = glfwGetKeyName(key, 0);
    if (!name) { name = "n/a"; }
    const Status glfw_status = platform::glfw::handle_error();
    if (glfw_status != Status::ok) {
        if (glfw_status == Status::invalid_value) {
            // Likely invalid scancode. Don't process it.
            return;
        } else {
            // @error: It failed, but there is not much we can do here because it's a void return
            // callback
            LOG_ERROR("GLFW failed with status '{}' while processing keyboard events",
                      to_string(glfw_status));
            return;
        }
    }
    LOG_INFO("Input key: {} {} (scancode {})", name, to_string_glfw_action(action), scancode);

    Game_Input& input_buf = Input_Manager::input_buffer;

    Game_Input_Controller& keyboard = input_buf.controllers[Controller::keyboard];
    Game_State& game_state = input_buf.state;
    Game_Settings& game_settings = input_buf.settings;

    // Ignore GLFW_REPEAT when processing physical keys.
    //
    // NOTE(sdsmith): A GLFW_RELEASE event will be sent once the key is released regardless of if
    // GLFW_REPEAT is sent.
    // ref: https://discourse.glfw.org/t/key-callback-not-registering-every-key-press/1438/4
    if (action == GLFW_REPEAT) { return; }

    const bool key_down = action == GLFW_PRESS;
    RK_ASSERT(key_down || action == GLFW_RELEASE);

    switch (key) {
            // case GLFW_KEY_SPACE:
        case GLFW_KEY_W: update_input_button(keyboard.up(), key_down); break;
        case GLFW_KEY_A: update_input_button(keyboard.left(), key_down); break;
        case GLFW_KEY_S: update_input_button(keyboard.down(), key_down); break;
        case GLFW_KEY_D: update_input_button(keyboard.right(), key_down); break;
        case GLFW_KEY_F7:
            if (!key_down) { game_settings.graphics.wireframe = !game_settings.graphics.wireframe; }
            break;
        case GLFW_KEY_F11:
            if (!key_down) { game_settings.window.fullscreen = !game_settings.window.fullscreen; }
            break;
        case GLFW_KEY_ESCAPE:
            if (!key_down) { game_state.request_quit = true; }
            break;
        case GLFW_KEY_UNKNOWN: LOG_INFO("Unknown key pressed with scancode {}", scancode); break;
        default: break;
    }
}

GLFWkeyfun Input_Manager::get_keyboard_event_callback() const noexcept
{
    return process_keyboard_event_callback;
}

Game_Input& Input_Manager::get_input() noexcept { return *m_new_input; }

Status Input_Manager::process_new_input() noexcept
{
    prepare_for_new_input();
    glfwPollEvents();
    RK_CHECK(platform::glfw::handle_error());

    // Update our new input with the input collected this cycle.
    *m_new_input = input_buffer;

    return Status::ok;
}

/**
 * \brief Prepare for the new input from a new cycle.
 */
void Input_Manager::prepare_for_new_input() noexcept
{
    // Save last input state (now old)
    std::swap(m_new_input, m_old_input);

    Game_State& new_game_state = m_new_input->state;
    Game_Settings& new_game_settings = m_new_input->settings;
    Game_Settings& old_game_settings = m_old_input->settings;
    Game_Input_Controller& new_keyboard = m_new_input->controllers[Controller::keyboard];
    Game_Input_Controller& old_keyboard = m_old_input->controllers[Controller::keyboard];

    // Carry the settings over
    // TODO(sdsmith): @perf: Should settings be their own object?
    new_game_settings = old_game_settings;

    // Zero new keyboard
    new_keyboard = zeroed_input.controllers[Controller::keyboard];

    Game_Input_Button* new_button = nullptr;
    Game_Input_Button* old_button = nullptr;

    for (s32 button_index = 0; button_index < new_keyboard.buttons.size(); ++button_index) {
        new_button = &new_keyboard.buttons[button_index];
        old_button = &old_keyboard.buttons[button_index];

        // Start down if ended down on last input
        new_button->started_down = old_button->ended_down;

        // Since input has not been processed yet, there are no half transitions
        // ie. We ended down if we started down (updated during input
        // processing)
        new_button->ended_down = new_button->started_down;
    }

    // Initialize the input buffer with any relevant state from the last cycle
    Input_Manager::input_buffer = *m_new_input;
}
