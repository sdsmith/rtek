#pragma once

#include "core/types.h"
#include "core/utility/assert.h"
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
    bool toggle_pause;
};

struct Game_Input {
    Game_State state;
    std::array<Game_Input_Controller, 1> controllers;
};
} // namespace rk