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
    std::unordered_map<InputKey, InputDeviceState> GetGamepadState(HID_INPUT::Input& input, const GLFWgamepadstate& state) {
        std::unordered_map<InputKey, InputDeviceState> gamepadState {};

        for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++) {
            int buttonState = state.buttons[i];
            float value = buttonState == GLFW_PRESS ? 1.f : 0.f;

            switch (i) {
                case GLFW_GAMEPAD_BUTTON_A:
                    gamepadState[InputKey::GAMEPAD_SOUTH].value = value;
                    break;
                case GLFW_GAMEPAD_BUTTON_B:
                    gamepadState[InputKey::GAMEPAD_EAST].value = value;
                    break;
                case GLFW_GAMEPAD_BUTTON_X:
                    gamepadState[InputKey::GAMEPAD_WEST].value = value;
                    break;
                case GLFW_GAMEPAD_BUTTON_Y:
                    gamepadState[InputKey::GAMEPAD_NORTH].value = value;
                    break;
                case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER:
                    gamepadState[InputKey::GAMEPAD_LB].value = value;
                    break;
                case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER:
                    gamepadState[InputKey::GAMEPAD_RB].value = value;
                    break;
                case GLFW_GAMEPAD_BUTTON_BACK:
                    gamepadState[InputKey::GAMEPAD_SELECT].value = value;
                    break;
                case GLFW_GAMEPAD_BUTTON_START:
                    gamepadState[InputKey::GAMEPAD_START].value = value;
                    break;
                case  GLFW_GAMEPAD_BUTTON_LEFT_THUMB:
                    gamepadState[InputKey::GAMEPAD_LT].value = value;
                    break;
                case  GLFW_GAMEPAD_BUTTON_RIGHT_THUMB:
                    gamepadState[InputKey::GAMEPAD_RT].value = value;
                    break;
                case  GLFW_GAMEPAD_BUTTON_DPAD_UP:
                    gamepadState[InputKey::GAMEPAD_DPAD_UP].value = value;
                    break;
                case  GLFW_GAMEPAD_BUTTON_DPAD_RIGHT:
                    gamepadState[InputKey::GAMEPAD_DPAD_RIGHT].value = value;
                    break;
                case  GLFW_GAMEPAD_BUTTON_DPAD_DOWN:
                    gamepadState[InputKey::GAMEPAD_DPAD_DOWN].value = value;
                    break;
                case  GLFW_GAMEPAD_BUTTON_DPAD_LEFT:
                    gamepadState[InputKey::GAMEPAD_DPAD_LEFT].value = value;
                case GLFW_GAMEPAD_BUTTON_GUIDE:
                default:
                    break;
            }
        }

        for (int i = 0; i <= GLFW_GAMEPAD_AXIS_LAST; i++) {
            float value = state.axes[i];
            switch (i) {
                case GLFW_GAMEPAD_AXIS_LEFT_X:
                    gamepadState[InputKey::GAMEPAD_L_THUMB_X].value = value;
                    break;
                case GLFW_GAMEPAD_AXIS_LEFT_Y:
                    gamepadState[InputKey::GAMEPAD_L_THUMB_Y].value = value;
                    break;
                case GLFW_GAMEPAD_AXIS_RIGHT_X:
                    gamepadState[InputKey::GAMEPAD_R_THUMB_X].value = value;
                    break;
                case GLFW_GAMEPAD_AXIS_RIGHT_Y:
                    gamepadState[InputKey::GAMEPAD_R_THUMB_Y].value = value;
                    break;
                case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER:
                    gamepadState[InputKey::GAMEPAD_L3].value = value;
                    break;
                case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER:
                    gamepadState[InputKey::GAMEPAD_R3].value = value;
                    break;
                default:
                    break;
            }
        }

        return gamepadState;
    }
    std::unordered_map<InputKey, InputDeviceState> GetGamepadStateById(HID_INPUT::Input& input, int joystickId) {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(joystickId, &state)) {
            return GetGamepadState(input, state);
        }
        return std::unordered_map<InputKey, InputDeviceState>{};
    }
    static InputKey KeyToInputKey(HID_INPUT::Input& input, int key)
    {
        switch (key) {
            case GLFW_KEY_A:
                return InputKey::KEYBOARD_A;
            case GLFW_KEY_B:
                return InputKey::KEYBOARD_B;
            case GLFW_KEY_C:
                return InputKey::KEYBOARD_C;
            case GLFW_KEY_D:
                return InputKey::KEYBOARD_D;
            case GLFW_KEY_E:
                return InputKey::KEYBOARD_E;
            case GLFW_KEY_S:
                return InputKey::KEYBOARD_S;
            case GLFW_KEY_W:
                return InputKey::KEYBOARD_W;

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

    void UpdateGamepadState(const GLFWgamepadstate& state) {

    }
}