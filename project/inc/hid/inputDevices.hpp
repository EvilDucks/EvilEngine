//
// Created by Bartek on 23.03.2024.
//

#ifndef EVILENGINE_INPUTDEVICES_HPP
#define EVILENGINE_INPUTDEVICES_HPP

#endif //EVILENGINE_INPUTDEVICES_HPP
#include <iostream>
#include <unordered_map>
#include <functional>


enum class InputDeviceType {
    KEYBOARD,
    MOUSE,
    GAMEPAD
};

struct InputDeviceState {
    float value { -99.f };
};

using InputDeviceStateCallbackFunc = std::function<std::unordered_map<InputKey, InputDeviceState>(int)>;

struct InputDevice {
    InputDeviceType type;
    int Index;
    std::unordered_map<InputKey, InputDeviceState> CurrentState;
    InputDeviceStateCallbackFunc StateFunc;
};