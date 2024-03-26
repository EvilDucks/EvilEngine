#include <utility>

//
// Created by Bartek on 22.03.2024.
//

#ifndef EVILENGINE_INPUTKEY_HPP
#define EVILENGINE_INPUTKEY_HPP

#endif //EVILENGINE_INPUTKEY_HPP


enum class InputKey {
    UNKNOWN,
    A,
    B,
    C,
    D,
    E,
    S,
    W,
    L_THUMB_X,
    L_THUMB_Y,
    R_THUMB_X,
    R_THUMB_Y,
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
