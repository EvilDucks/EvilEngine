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
        std::unordered_map<InputKey, InputContext> _keyInputContext {};

        std::unordered_map<int, std::unordered_map<InputKey, InputDeviceState>> _gamepadStates {};
    };
    using Input = Base*;

    void Create (HID_INPUT::Input & input) {
        input = new Base;
    }

    void Destroy (HID_INPUT::Input & input) {
        delete input;
    }

    void HandleGamepadContext(HID_INPUT::Input& input, InputKey key, InputContext context)
    {
        if (context == InputContext::STARTED && input->_keyInputContext[key] != InputContext::CANCELED)
        {
            context = InputContext::REPEATED;
        }
        input->_keyInputContext[key] = context;
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
            float value = 0.f;

            switch (buttonState) {
                case GLFW_PRESS:
                    value = 1.f;
                    break;
                case GLFW_REPEAT:
                    // GLFW does not detect GLFW_REPEAT for gamepad
                    value = 1.f;
                    break;
                default:
                    value = 0.f;
            }

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
            case GLFW_KEY_SPACE:
                return InputKey::KEYBOARD_SPACE;
            case GLFW_KEY_APOSTROPHE:
                return InputKey::KEYBOARD_APOSTROPHE;
            case GLFW_KEY_COMMA:
                return InputKey::KEYBOARD_COMMA;
            case GLFW_KEY_MINUS:
                return InputKey::KEYBOARD_MINUS;
            case GLFW_KEY_PERIOD:
                return InputKey::KEYBOARD_PERIOD;
            case GLFW_KEY_SLASH:
                return InputKey::KEYBOARD_SLASH;
            case GLFW_KEY_0:
                return InputKey::KEYBOARD_0;
            case GLFW_KEY_1:
                return InputKey::KEYBOARD_1;
            case GLFW_KEY_2:
                return InputKey::KEYBOARD_2;
            case GLFW_KEY_3:
                return InputKey::KEYBOARD_3;
            case GLFW_KEY_4:
                return InputKey::KEYBOARD_4;
            case GLFW_KEY_5:
                return InputKey::KEYBOARD_5;
            case GLFW_KEY_6:
                return InputKey::KEYBOARD_6;
            case GLFW_KEY_7:
                return InputKey::KEYBOARD_7;
            case GLFW_KEY_8:
                return InputKey::KEYBOARD_8;
            case GLFW_KEY_9:
                return InputKey::KEYBOARD_9;
            case GLFW_KEY_SEMICOLON:
                return InputKey::KEYBOARD_SEMICOLON;
            case GLFW_KEY_EQUAL:
                return InputKey::KEYBOARD_EQUAL;
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
            case GLFW_KEY_F:
                return InputKey::KEYBOARD_F;
            case GLFW_KEY_G:
                return InputKey::KEYBOARD_G;
            case GLFW_KEY_H:
                return InputKey::KEYBOARD_H;
            case GLFW_KEY_I:
                return InputKey::KEYBOARD_I;
            case GLFW_KEY_J:
                return InputKey::KEYBOARD_J;
            case GLFW_KEY_K:
                return InputKey::KEYBOARD_K;
            case GLFW_KEY_L:
                return InputKey::KEYBOARD_L;
            case GLFW_KEY_M:
                return InputKey::KEYBOARD_M;
            case GLFW_KEY_N:
                return InputKey::KEYBOARD_N;
            case GLFW_KEY_O:
                return InputKey::KEYBOARD_O;
            case GLFW_KEY_P :
                return InputKey::KEYBOARD_P;
            case GLFW_KEY_Q:
                return InputKey::KEYBOARD_Q;
            case GLFW_KEY_R:
                return InputKey::KEYBOARD_R;
            case GLFW_KEY_S:
                return InputKey::KEYBOARD_S;
            case GLFW_KEY_T:
                return InputKey::KEYBOARD_T;
            case GLFW_KEY_U:
                return InputKey::KEYBOARD_U;
            case GLFW_KEY_V:
                return InputKey::KEYBOARD_V;
            case GLFW_KEY_W:
                return InputKey::KEYBOARD_W;
            case GLFW_KEY_X:
                return InputKey::KEYBOARD_X;
            case GLFW_KEY_Y:
                return InputKey::KEYBOARD_Y;
            case GLFW_KEY_Z:
                return InputKey::KEYBOARD_Z;
            case GLFW_KEY_LEFT_BRACKET:
                return InputKey::KEYBOARD_LEFT_BRACKET;
            case GLFW_KEY_BACKSLASH:
                return InputKey::KEYBOARD_BACKSLASH;
            case GLFW_KEY_RIGHT_BRACKET:
                return InputKey::KEYBOARD_RIGHT_BRACKET;
            case GLFW_KEY_GRAVE_ACCENT:
                return InputKey::KEYBOARD_GRAVE_ACCENT;
            case GLFW_KEY_WORLD_1:
                return InputKey::KEYBOARD_WORLD_1;
            case GLFW_KEY_WORLD_2:
                return InputKey::KEYBOARD_WORLD_2;

/* Function keys */
            case GLFW_KEY_ESCAPE:
                return InputKey::KEYBOARD_ESCAPE;
            case GLFW_KEY_ENTER:
                return InputKey::KEYBOARD_ENTER;
            case GLFW_KEY_TAB:
                return InputKey::KEYBOARD_TAB;
            case GLFW_KEY_BACKSPACE:
                return InputKey::KEYBOARD_BACKSPACE;
            case GLFW_KEY_INSERT:
                return InputKey::KEYBOARD_INSERT;
            case GLFW_KEY_DELETE:
                return InputKey::KEYBOARD_DELETE;
            case GLFW_KEY_RIGHT:
                return InputKey::KEYBOARD_RIGHT;
            case GLFW_KEY_LEFT:
                return InputKey::KEYBOARD_LEFT;
            case GLFW_KEY_DOWN:
                return InputKey::KEYBOARD_DOWN;
            case GLFW_KEY_UP:
                return InputKey::KEYBOARD_UP;
            case GLFW_KEY_PAGE_UP:
                return InputKey::KEYBOARD_PAGE_UP;
            case GLFW_KEY_PAGE_DOWN:
                return InputKey::KEYBOARD_PAGE_DOWN;
            case GLFW_KEY_HOME:
                return InputKey::KEYBOARD_HOME;
            case GLFW_KEY_END:
                return InputKey::KEYBOARD_END;
            case GLFW_KEY_CAPS_LOCK:
                return InputKey::KEYBOARD_CAPS_LOCK;
            case GLFW_KEY_SCROLL_LOCK:
                return InputKey::KEYBOARD_SCROLL_LOCK;
            case GLFW_KEY_NUM_LOCK:
                return InputKey::KEYBOARD_NUM_LOCK;
            case GLFW_KEY_PRINT_SCREEN:
                return InputKey::KEYBOARD_PRINT_SCREEN;
            case GLFW_KEY_PAUSE:
                return InputKey::KEYBOARD_PAUSE;
            case GLFW_KEY_F1:
                return InputKey::KEYBOARD_F1;
            case GLFW_KEY_F2:
                return InputKey::KEYBOARD_F2;
            case GLFW_KEY_F3:
                return InputKey::KEYBOARD_F3;
            case GLFW_KEY_F4:
                return InputKey::KEYBOARD_F4;
            case GLFW_KEY_F5:
                return InputKey::KEYBOARD_F5;
            case GLFW_KEY_F6:
                return InputKey::KEYBOARD_F6;
            case GLFW_KEY_F7:
                return InputKey::KEYBOARD_F7;
            case GLFW_KEY_F8:
                return InputKey::KEYBOARD_F8;
            case GLFW_KEY_F9:
                return InputKey::KEYBOARD_F9;
            case GLFW_KEY_F10:
                return InputKey::KEYBOARD_F10;
            case GLFW_KEY_F11:
                return InputKey::KEYBOARD_F11;
            case GLFW_KEY_F12:
                return InputKey::KEYBOARD_F12;
            case GLFW_KEY_F13:
                return InputKey::KEYBOARD_F13;
            case GLFW_KEY_F14:
                return InputKey::KEYBOARD_F14;
            case GLFW_KEY_F15:
                return InputKey::KEYBOARD_F15;
            case GLFW_KEY_F16:
                return InputKey::KEYBOARD_F16;
            case GLFW_KEY_F17:
                return InputKey::KEYBOARD_F17;
            case GLFW_KEY_F18:
                return InputKey::KEYBOARD_F18;
            case GLFW_KEY_F19:
                return InputKey::KEYBOARD_F19;
            case GLFW_KEY_F20:
                return InputKey::KEYBOARD_F20;
            case GLFW_KEY_F21:
                return InputKey::KEYBOARD_F21;
            case GLFW_KEY_F22:
                return InputKey::KEYBOARD_F22;
            case GLFW_KEY_F23:
                return InputKey::KEYBOARD_F23;
            case GLFW_KEY_F24:
                return InputKey::KEYBOARD_F24;
            case GLFW_KEY_F25:
                return InputKey::KEYBOARD_F25;
            case GLFW_KEY_KP_0:
                return InputKey::KEYBOARD_KP_0;
            case GLFW_KEY_KP_1:
                return InputKey::KEYBOARD_KP_1;
            case GLFW_KEY_KP_2:
                return InputKey::KEYBOARD_KP_2;
            case GLFW_KEY_KP_3:
                return InputKey::KEYBOARD_KP_3;
            case GLFW_KEY_KP_4:
                return InputKey::KEYBOARD_KP_4;
            case GLFW_KEY_KP_5:
                return InputKey::KEYBOARD_KP_5;
            case GLFW_KEY_KP_6:
                return InputKey::KEYBOARD_KP_6;
            case GLFW_KEY_KP_7:
                return InputKey::KEYBOARD_KP_7;
            case GLFW_KEY_KP_8:
                return InputKey::KEYBOARD_KP_8;
            case GLFW_KEY_KP_9:
                return InputKey::KEYBOARD_KP_9;
            case GLFW_KEY_KP_DECIMAL:
                return InputKey::KEYBOARD_KP_DECIMAL;
            case GLFW_KEY_KP_DIVIDE:
                return InputKey::KEYBOARD_KP_DIVIDE;
            case GLFW_KEY_KP_MULTIPLY:
                return InputKey::KEYBOARD_KP_MULTIPLY;
            case GLFW_KEY_KP_SUBTRACT:
                return InputKey::KEYBOARD_KP_SUBTRACT;
            case GLFW_KEY_KP_ADD:
                return InputKey::KEYBOARD_KP_ADD;
            case GLFW_KEY_KP_ENTER:
                return InputKey::KEYBOARD_KP_ENTER;
            case GLFW_KEY_KP_EQUAL:
                return InputKey::KEYBOARD_KP_EQUAL;
            case GLFW_KEY_LEFT_SHIFT:
                return InputKey::KEYBOARD_LEFT_SHIFT;
            case GLFW_KEY_LEFT_CONTROL:
                return InputKey::KEYBOARD_LEFT_CONTROL;
            case GLFW_KEY_LEFT_ALT:
                return InputKey::KEYBOARD_LEFT_ALT;
            case GLFW_KEY_LEFT_SUPER:
                return InputKey::KEYBOARD_LEFT_SUPER;
            case GLFW_KEY_RIGHT_SHIFT:
                return InputKey::KEYBOARD_RIGHT_SHIFT;
            case GLFW_KEY_RIGHT_CONTROL:
                return InputKey::KEYBOARD_RIGHT_CONTROL;
            case GLFW_KEY_RIGHT_ALT:
                return InputKey::KEYBOARD_RIGHT_ALT;
            case GLFW_KEY_RIGHT_SUPER:
                return InputKey::KEYBOARD_RIGHT_SUPER;
            case GLFW_KEY_MENU:
                return InputKey::KEYBOARD_MENU;

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
        input->_mouseState[iKey].value = value;
    }

    void UpdateMouseCursorState(HID_INPUT::Input& input, float xPos, float yPos) {
        input->_mouseState[InputKey::MOUSE_POS_X].value = xPos;
        input->_mouseState[InputKey::MOUSE_POS_Y].value = yPos;
    }

    void UpdateGamepadState(const GLFWgamepadstate& state) {

    }
}