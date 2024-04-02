#include <utility>

//
// Created by Bartek on 22.03.2024.
//

#ifndef EVILENGINE_INPUTKEY_HPP
#define EVILENGINE_INPUTKEY_HPP

#endif //EVILENGINE_INPUTKEY_HPP


enum class InputKey {
    UNKNOWN,
    KEYBOARD_A,
    KEYBOARD_B,
    KEYBOARD_C,
    KEYBOARD_D,
    KEYBOARD_E,
    KEYBOARD_S,
    KEYBOARD_W,
    KEYBOARD_SPACEBAR,
    KEYBOARD_LSHIFT,
    GAMEPAD_L_THUMB_X,
    GAMEPAD_L_THUMB_Y,
    GAMEPAD_R_THUMB_X,
    GAMEPAD_R_THUMB_Y,
    GAMEPAD_NORTH,
    GAMEPAD_SOUTH,
    GAMEPAD_WEST,
    GAMEPAD_EAST,
    GAMEPAD_START,
    GAMEPAD_SELECT,
    GAMEPAD_RB,
    GAMEPAD_LB,
    GAMEPAD_RT,
    GAMEPAD_LT,
    GAMEPAD_DPAD_LEFT,
    GAMEPAD_DPAD_RIGHT,
    GAMEPAD_DPAD_UP,
    GAMEPAD_DPAD_DOWN,
    GAMEPAD_R3,
    GAMEPAD_L3,
    MOUSE_POS_X,
    MOUSE_POS_Y,
    MOUSE_MOVE_X,
    MOUSE_MOVE_Y,
    MOUSE_RIGHT,
    MOUSE_LEFT,
    MOUSE_MIDDLE
};


enum class InputSource {
    KEYBOARD,
    MOUSE,
    GAMEPAD,
    UNKNOWN
};

struct InputAction {
    std::string actionName;
    float scale { 1.f };

    explicit InputAction(std::string  name, float s = 1.f) : actionName(std::move(name)), scale(s) {}
};

InputSource GetInputSourceFromKey (InputKey key);
