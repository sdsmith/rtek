#include "core/core.h"

#include "core/hid/input.h"
#include "core/logging/logging.h"
#include "core/utility/assert.h"

// clang-format off
// NOTE(sdsmith): GLAD must be included before any other OpenGL related header.
// GLAD includes the OpenGL headers.
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

using namespace rk;

// Check that glfw primitive types match the engines
RK_STATIC_ASSERT_MSG(sizeof(int) == sizeof(s32), "glfw sizes don't match");

/*
 * NOTE(sdsmith): GLFW errors are always non-fatal. The lib will remain in a valid state as long as
 * glfwInit was successful.
 */

/**
 * \brief Window resize callback function.
 */
void framebuffer_size_callback(GLFWwindow* window, s32 width, s32 height)
{
    glViewport(0, 0, width, height);
}

/**
 * \brief Called on every glfw error when hooked up with \a glfwSetErrorCallback.
 */
void glfw_error_callback(s32 error_code, char const* description)
{
    // TODO(sdsmith): if using GLFW from multiple threads, needs to support that
    RK_ASSERT(description);
    RK_ASSERT(error_code != GLFW_NO_ERROR);
    LOG_ERROR("glfw failed with error code {}: {}", error_code, description);
}

/**
 * \brief Check for a GLFW error. Terminate on error. Return appropriate status code.
 */
Status handle_glfw_error()
{
    if (glfwGetError(nullptr) == GLFW_NO_ERROR) { return Status::ok; }

    glfwTerminate();
    return Status::renderer_error;
}

/**
 * \def RK_CHECK_GLFW
 * \brief Check for GLFW error. Handle and return on error.
 */
#define RK_CHECK_GLFW(func_call)       \
    do {                               \
        func_call;                     \
        RK_CHECK(handle_glfw_error()); \
    } while (0)

// TODO(sdsmith): use glfwSetCharCallback for unicode input for text

std::array<Game_Input, 2> input;
Game_Input* new_input = &input[0];
Game_Input* old_input = &input[1];
static const Game_Input zeroed_input = {};

Game_Input& get_input() noexcept { return *new_input; }

constexpr bool is_shift_key_active(s32 modifier_keys) noexcept
{
    return modifier_keys & GLFW_MOD_SHIFT;
}
constexpr bool is_ctrl_key_active(s32 modifier_keys) noexcept
{
    return modifier_keys & GLFW_MOD_CONTROL;
}
constexpr bool is_alt_key_active(s32 modifier_keys) noexcept
{
    return modifier_keys & GLFW_MOD_ALT;
}
constexpr bool is_super_key_active(s32 modifier_keys) noexcept
{
    return modifier_keys & GLFW_MOD_SUPER;
}
constexpr bool is_capslock_key_active(s32 modifier_keys)
{
    return modifier_keys & GLFW_MOD_CAPS_LOCK;
}
constexpr bool is_numlock_key_active(s32 modifier_keys)
{
    return modifier_keys & GLFW_MOD_NUM_LOCK;
}

/**
 * \brief Prepare for the new input from a new cycle.
 */
void prepare_for_new_input() noexcept
{
    // Save last input state (now old)
    std::swap(new_input, old_input);

    Game_State& new_game_state = new_input->state;
    Game_Input_Controller& new_keyboard = new_input->controllers[Controller::keyboard];
    Game_Input_Controller& old_keyboard = old_input->controllers[Controller::keyboard];

    // Reset toggles
    new_game_state.toggle_pause = false;

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
}

/**
 * \brief Update button with state change.
 */
void update_input_button(Game_Input_Button& button, bool button_down) noexcept
{
    button.ended_down = button_down;
    button.half_transitions++;
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
void process_keyboard_event_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action,
                                     s32 mods)
{
    RK_ASSERT(window);

    // action: GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT

    Game_Input_Controller& keyboard = new_input->controllers[Controller::keyboard];
    Game_State& game_state = new_input->state;

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
        case GLFW_KEY_ESCAPE:
            if (!key_down) { game_state.request_quit = true; }
            break;
        case GLFW_KEY_UNKNOWN: LOG_INFO("Unknown key pressed with scancode {}", scancode); break;
        default: break;
    }
}

Status Rtek_Engine::initialize()
{
    RK_CHECK(Logger::initialize());
    LOG_INFO("Logger initialized");
    LOG_INFO("Initializing engine...");

    LOG_INFO("Initializing OpenGL context...");
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();

    constexpr s32 glctx_ver_major = 4;
    constexpr s32 glctx_ver_minor = 6;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glctx_ver_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glctx_ver_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    handle_glfw_error();

    constexpr s32 window_w = 800;
    constexpr s32 window_h = 600;
    GLFWwindow* window = glfwCreateWindow(window_w, window_h, "RTek", nullptr, nullptr);
    if (!window) {
        LOG_CRITICAL("Failed to create window");
        glfwTerminate(); // TODO(sdsmith): this should be done any error after glfw is initialized
        return Status::renderer_error;
    }
    RK_CHECK_GLFW(glfwMakeContextCurrent(window));
    RK_CHECK_GLFW(glfwSetFramebufferSizeCallback(window, framebuffer_size_callback));
    glfwSetKeyCallback(window, process_keyboard_event_callback);
    handle_glfw_error();

    // Initialize GLAD with the OS-specific OGL func pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        LOG_CRITICAL("Failed to initialize GLAD");
        return Status::renderer_error;
    }
    glViewport(0, 0, window_w, window_h);
    LOG_INFO("OpenGL context initialized: OpenGL {}.{} core", glctx_ver_major, glctx_ver_minor);

    // TODO(sdsmith): move to `run`
    // TODO(sdsmith): probs don't want to rely on glfwwindowshouldclose
    bool running = true;
    while (!glfwWindowShouldClose(window) && running) {
        prepare_for_new_input();
        glfwPollEvents();

        Game_Input const input = get_input();
        running = !input.state.request_quit;
        if (!running) { break; }

        glClearColor(0.4f, 0.4f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        handle_glfw_error();
    }

    LOG_INFO("Engine initialized");
    m_initialized = true;
    return Status::ok;
}

Status Rtek_Engine::destroy()
{
    if (!m_initialized) {
        LOG_ERROR("Attempt to destroy uninitialized engine");
        return Status::api_error;
    }

    glfwTerminate();

    m_initialized = false;
    LOG_INFO("Engine destroyed");
    Logger::flush();
    return Status::ok;
}

Status Rtek_Engine::run() { return Status::ok; }

bool Rtek_Engine::m_initialized = false;
