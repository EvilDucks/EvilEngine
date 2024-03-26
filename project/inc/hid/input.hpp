//
// Created by Bartek on 23.03.2024.
//

#ifndef EVILENGINE_INPUT_HPP
#define EVILENGINE_INPUT_HPP

#endif //EVILENGINE_INPUT_HPP

#include <unordered_map>
#include <GLFW/glfw3.h>

namespace HID_INPUT {

    struct Base {
        std::unordered_map<InputKey, InputDeviceState> _keyboardState {};
        std::unordered_map<InputKey, InputDeviceState> _mouseState {};

        std::unordered_map<int, std::unordered_map<InputKey, InputDeviceState>> _gamepadStates {};
    };
    using Input = Base*;

    void Create (HID_INPUT::Input & input) {
        input = new Base;
    }

    void Destroy (HID_INPUT::Input & input) {
        delete input;
    }

    std::unordered_map<InputKey, InputDeviceState> GetKeyboardState(HID_INPUT::Input& input, int index)
    {
        return input->_keyboardState;
    }
    std::unordered_map<InputKey, InputDeviceState> GetMouseState(HID_INPUT::Input& input, int index) { return input->_mouseState; }
    std::unordered_map<InputKey, InputDeviceState> GetGamepadState(HID_INPUT::Input& input, int index) { return input->_gamepadStates[index]; }

    static InputKey KeyToInputKey(HID_INPUT::Input& input, int key)
    {
        switch (key) {
            case GLFW_KEY_A:
                return InputKey::A;
            case GLFW_KEY_B:
                return InputKey::B;
            case GLFW_KEY_C:
                return InputKey::C;
            case GLFW_KEY_D:
                return InputKey::D;
            case GLFW_KEY_E:
                return InputKey::E;
            case GLFW_KEY_S:
                return InputKey::S;
            case GLFW_KEY_W:
                return InputKey::W;

            default:
                return InputKey::UNKNOWN;

        }
    }

    static InputKey MouseButtonToInputKey(HID_INPUT::Input& input, int key)
    {
        switch (key) {
            case GLFW_MOUSE_BUTTON_LEFT:
                return InputKey::MOUSE_LEFT;
            case GLFW_MOUSE_BUTTON_RIGHT:
                return InputKey::MOUSE_RIGHT;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                return InputKey::MOUSE_MIDDLE;

            default:
                return InputKey::UNKNOWN;

        }
    }

    void UpdateKeyboardState(HID_INPUT::Input& input, int key, float value) {
        InputKey iKey = KeyToInputKey(input, key);
        input->_keyboardState[iKey].value = value;
    }

    void UpdateMouseState(HID_INPUT::Input& input, int button, float value) {
        InputKey iKey = MouseButtonToInputKey(input, button);
        input->_keyboardState[iKey].value = value;
    }
}