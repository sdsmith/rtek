#pragma once

#include "core/types.h"
#include <array>
#include <cstddef>

namespace rk
{
using Controller_Id = s32;

enum Controller { keyboard = 0 };

struct Game_Input_Button {
    bool started_down = false;
    bool ended_down = false;
    s32 half_transitions = 0;
};

struct Game_Input_Controller {
    std::array<Game_Input_Button, 4> buttons;

    Game_Input_Button& up() { return buttons[0]; }
    Game_Input_Button& down() { return buttons[1]; }
    Game_Input_Button& left() { return buttons[2]; }
    Game_Input_Button& right() { return buttons[3]; }
};

struct Game_State {
    bool request_quit;
};

struct Window_Settings {
    bool fullscreen = false;
};

struct Graphics_Settings {
    /** Draw wirefram objects. */
    bool wireframe = false;
};

struct Game_Settings {
    Window_Settings window;
    Graphics_Settings graphics;
};

struct Game_Input {
    Game_State state;
    Game_Settings settings;
    std::array<Game_Input_Controller, 1> controllers;
};
} // namespace rk
