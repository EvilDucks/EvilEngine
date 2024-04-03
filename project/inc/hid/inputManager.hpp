//
// Created by Bartek on 22.03.2024.
//

#ifndef EVILENGINE_INPUTMANAGER_H
#define EVILENGINE_INPUTMANAGER_H

#endif //EVILENGINE_INPUTMANAGER_H

#include "inputKey.hpp"
#include "inputDevices.hpp"
#include "global.hpp"
#include "input.hpp"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <functional>
#include <string>


namespace INPUT_MANAGER {

    InputSource GetInputSourceFromKey (InputKey key) {
        switch (key) {
            case InputKey::KEYBOARD_SPACE:
            case InputKey::KEYBOARD_APOSTROPHE:
            case InputKey::KEYBOARD_COMMA:
            case InputKey::KEYBOARD_MINUS:
            case InputKey::KEYBOARD_PERIOD:
            case InputKey::KEYBOARD_SLASH:
            case InputKey::KEYBOARD_0:
            case InputKey::KEYBOARD_1:
            case InputKey::KEYBOARD_2:
            case InputKey::KEYBOARD_3:
            case InputKey::KEYBOARD_4:
            case InputKey::KEYBOARD_5:
            case InputKey::KEYBOARD_6:
            case InputKey::KEYBOARD_7:
            case InputKey::KEYBOARD_8:
            case InputKey::KEYBOARD_9:
            case InputKey::KEYBOARD_SEMICOLON:
            case InputKey::KEYBOARD_EQUAL:
            case InputKey::KEYBOARD_A:
            case InputKey::KEYBOARD_B:
            case InputKey::KEYBOARD_C:
            case InputKey::KEYBOARD_D:
            case InputKey::KEYBOARD_E:
            case InputKey::KEYBOARD_F:
            case InputKey::KEYBOARD_G:
            case InputKey::KEYBOARD_H:
            case InputKey::KEYBOARD_I:
            case InputKey::KEYBOARD_J:
            case InputKey::KEYBOARD_K:
            case InputKey::KEYBOARD_L:
            case InputKey::KEYBOARD_M:
            case InputKey::KEYBOARD_N:
            case InputKey::KEYBOARD_O:
            case InputKey::KEYBOARD_P :
            case InputKey::KEYBOARD_Q:
            case InputKey::KEYBOARD_R:
            case InputKey::KEYBOARD_S:
            case InputKey::KEYBOARD_T:
            case InputKey::KEYBOARD_U:
            case InputKey::KEYBOARD_V:
            case InputKey::KEYBOARD_W:
            case InputKey::KEYBOARD_X:
            case InputKey::KEYBOARD_Y:
            case InputKey::KEYBOARD_Z:
            case InputKey::KEYBOARD_LEFT_BRACKET:
            case InputKey::KEYBOARD_BACKSLASH:
            case InputKey::KEYBOARD_RIGHT_BRACKET:
            case InputKey::KEYBOARD_GRAVE_ACCENT:
            case InputKey::KEYBOARD_WORLD_1:
            case InputKey::KEYBOARD_WORLD_2:
            case InputKey::KEYBOARD_ESCAPE:
            case InputKey::KEYBOARD_ENTER:
            case InputKey::KEYBOARD_TAB:
            case InputKey::KEYBOARD_BACKSPACE:
            case InputKey::KEYBOARD_INSERT:
            case InputKey::KEYBOARD_DELETE:
            case InputKey::KEYBOARD_RIGHT:
            case InputKey::KEYBOARD_LEFT:
            case InputKey::KEYBOARD_DOWN:
            case InputKey::KEYBOARD_UP:
            case InputKey::KEYBOARD_PAGE_UP:
            case InputKey::KEYBOARD_PAGE_DOWN:
            case InputKey::KEYBOARD_HOME:
            case InputKey::KEYBOARD_END:
            case InputKey::KEYBOARD_CAPS_LOCK:
            case InputKey::KEYBOARD_SCROLL_LOCK:
            case InputKey::KEYBOARD_NUM_LOCK:
            case InputKey::KEYBOARD_PRINT_SCREEN:
            case InputKey::KEYBOARD_PAUSE:
            case InputKey::KEYBOARD_F1:
            case InputKey::KEYBOARD_F2:
            case InputKey::KEYBOARD_F3:
            case InputKey::KEYBOARD_F4:
            case InputKey::KEYBOARD_F5:
            case InputKey::KEYBOARD_F6:
            case InputKey::KEYBOARD_F7:
            case InputKey::KEYBOARD_F8:
            case InputKey::KEYBOARD_F9:
            case InputKey::KEYBOARD_F10:
            case InputKey::KEYBOARD_F11:
            case InputKey::KEYBOARD_F12:
            case InputKey::KEYBOARD_F13:
            case InputKey::KEYBOARD_F14:
            case InputKey::KEYBOARD_F15:
            case InputKey::KEYBOARD_F16:
            case InputKey::KEYBOARD_F17:
            case InputKey::KEYBOARD_F18:
            case InputKey::KEYBOARD_F19:
            case InputKey::KEYBOARD_F20:
            case InputKey::KEYBOARD_F21:
            case InputKey::KEYBOARD_F22:
            case InputKey::KEYBOARD_F23:
            case InputKey::KEYBOARD_F24:
            case InputKey::KEYBOARD_F25:
            case InputKey::KEYBOARD_KP_0:
            case InputKey::KEYBOARD_KP_1:
            case InputKey::KEYBOARD_KP_2:
            case InputKey::KEYBOARD_KP_3:
            case InputKey::KEYBOARD_KP_4:
            case InputKey::KEYBOARD_KP_5:
            case InputKey::KEYBOARD_KP_6:
            case InputKey::KEYBOARD_KP_7:
            case InputKey::KEYBOARD_KP_8:
            case InputKey::KEYBOARD_KP_9:
            case InputKey::KEYBOARD_KP_DECIMAL:
            case InputKey::KEYBOARD_KP_DIVIDE:
            case InputKey::KEYBOARD_KP_MULTIPLY:
            case InputKey::KEYBOARD_KP_SUBTRACT:
            case InputKey::KEYBOARD_KP_ADD:
            case InputKey::KEYBOARD_KP_ENTER:
            case InputKey::KEYBOARD_KP_EQUAL:
            case InputKey::KEYBOARD_LEFT_SHIFT:
            case InputKey::KEYBOARD_LEFT_CONTROL:
            case InputKey::KEYBOARD_LEFT_ALT:
            case InputKey::KEYBOARD_LEFT_SUPER:
            case InputKey::KEYBOARD_RIGHT_SHIFT:
            case InputKey::KEYBOARD_RIGHT_CONTROL:
            case InputKey::KEYBOARD_RIGHT_ALT:
            case InputKey::KEYBOARD_RIGHT_SUPER:
            case InputKey::KEYBOARD_MENU:
                return InputSource::KEYBOARD;
            case InputKey::GAMEPAD_L_THUMB_X:
            case InputKey::GAMEPAD_L_THUMB_Y:
            case InputKey::GAMEPAD_R_THUMB_X:
            case InputKey::GAMEPAD_R_THUMB_Y:
            case InputKey::GAMEPAD_NORTH:
            case InputKey::GAMEPAD_SOUTH:
            case InputKey::GAMEPAD_WEST:
            case InputKey::GAMEPAD_EAST:
            case InputKey::GAMEPAD_START:
            case InputKey::GAMEPAD_SELECT:
            case InputKey::GAMEPAD_RB:
            case InputKey::GAMEPAD_LB:
            case InputKey::GAMEPAD_RT:
            case InputKey::GAMEPAD_LT:
            case InputKey::GAMEPAD_DPAD_LEFT:
            case InputKey::GAMEPAD_DPAD_RIGHT:
            case InputKey::GAMEPAD_DPAD_UP:
            case InputKey::GAMEPAD_DPAD_DOWN:
            case InputKey::GAMEPAD_R3:
            case InputKey::GAMEPAD_L3:
                return InputSource::GAMEPAD;
            case InputKey::MOUSE_LEFT:
            case InputKey::MOUSE_MIDDLE:
            case InputKey::MOUSE_RIGHT:
            case InputKey::MOUSE_MOVE_X:
            case InputKey::MOUSE_MOVE_Y:
            case InputKey::MOUSE_POS_X:
            case InputKey::MOUSE_POS_Y:
                return InputSource::MOUSE;
            default:
                return InputSource::UNKNOWN;

        }
    }

    using ActionCallbackFunc = std::function<bool(InputSource, int, float, InputContext)>;

    struct ActionCallback {
        std::string Ref;
        ActionCallbackFunc Func;
    };

    struct ActionEvent {
        std::string ActionName;
        InputSource Source;
        int SourceIndex;
        float Value;
        InputContext Context;
    };

    struct InputManager {
        ActionCallback actionCallback;
        ActionEvent actionEvent;
        std::unordered_map<InputKey, std::vector<InputAction>> _inputActionMapping {};
        std::unordered_map<std::string, std::vector<ActionCallback>> _actionCallbacks {};
        std::vector<InputDevice> _devices;
    };
    using IM = InputManager*;

    void Create (INPUT_MANAGER::IM& inputManager);
    void RegisterActionCallback (INPUT_MANAGER::IM inputManager, const std::string& actionName, const ActionCallback& callback);
    void RemoveActionCallback (INPUT_MANAGER::IM inputManager, const std::string& actionName, const std::string& callbackRef);
    void MapInputToAction (INPUT_MANAGER::IM inputManager, InputKey key, const InputAction& action);
    void UnmapInputFromAction (INPUT_MANAGER::IM inputManager, InputKey key, const std::string& action);
    void ProcessInput (INPUT_MANAGER::IM inputManager, HID_INPUT::Input input);
    std::vector<ActionEvent> GenerateActionEvent (INPUT_MANAGER::IM inputManager, HID_INPUT::Input input, int deviceIndex, InputKey key, float newValue);
    void PropagateActionEvent (INPUT_MANAGER::IM inputManager, ActionEvent event);
    void RegisterDevice (INPUT_MANAGER::IM inputManager, const InputDevice& device);
    void RemoveDevice (INPUT_MANAGER::IM inputManager, InputDeviceType type, int inputIndex);


    void Create (INPUT_MANAGER::IM& inputManager) {
        DEBUG { spdlog::info ("Input Manager Created");}
        inputManager = new InputManager;
    }

    void Destroy (INPUT_MANAGER::IM inputManager) {
        delete inputManager;
    }

    void RegisterActionCallback (INPUT_MANAGER::IM inputManager, const std::string& actionName, const ActionCallback& callback) {
        inputManager->_actionCallbacks[actionName].emplace_back(callback);

    }

    void RemoveActionCallback (INPUT_MANAGER::IM inputManager, const std::string& actionName, const std::string& callbackRef) {
        erase_if(inputManager->_actionCallbacks[actionName], [callbackRef](const ActionCallback& callback) {
            return callback.Ref == callbackRef;
        });

    }

    void MapInputToAction (INPUT_MANAGER::IM inputManager, InputKey key, const InputAction& action) {
        UnmapInputFromAction(inputManager, key, action.actionName);
        inputManager->_inputActionMapping[key].emplace_back(action);
    }

    void UnmapInputFromAction (INPUT_MANAGER::IM inputManager, InputKey key, const std::string& action) {
        inputManager->_inputActionMapping[key].erase(std::remove_if(inputManager->_inputActionMapping[key].begin(), inputManager->_inputActionMapping[key].end(),
        [&action](const InputAction& inputAction) {
                return inputAction.actionName == action;
             }), inputManager->_inputActionMapping[key].end());
    }

    void ProcessInput (INPUT_MANAGER::IM inputManager, HID_INPUT::Input input) {
        std::vector<ActionEvent> events {};
        for (auto& device : inputManager->_devices) {
            auto newState = device.StateFunc(device.Index);

            for (auto& keyState : newState) {
                if (device.CurrentState[keyState.first].value != keyState.second.value || abs(keyState.second.value) > 0.1f) {
                    if (device.CurrentState[keyState.first].value != keyState.second.value)
                    {
                        if (abs(keyState.second.value) < 0.1f)
                        {
                            input->_keyInputContext[keyState.first] = InputContext::CANCELED;
                        }
                        else
                        {
                            input->_keyInputContext[keyState.first] = InputContext::STARTED;
                        }
                    }
                    else
                    {
                        input->_keyInputContext[keyState.first] = InputContext::REPEATED;
                    }
                    auto generatedEvents = GenerateActionEvent(inputManager, input, device.Index, keyState.first, keyState.second.value);
                    events.insert(events.end(), generatedEvents.begin(), generatedEvents.end());

                    device.CurrentState[keyState.first].value = keyState.second.value;
                }
            }
        }
        for (auto& event : events) {
            PropagateActionEvent(inputManager, event);
        }

    }

    std::vector<ActionEvent> GenerateActionEvent (INPUT_MANAGER::IM inputManager, HID_INPUT::Input input, int deviceIndex, InputKey key, float newValue) {
        auto& actions = inputManager->_inputActionMapping[key];

        std::vector<ActionEvent> actionEvents {};

        InputSource source = INPUT_MANAGER::GetInputSourceFromKey(key);

        for (auto& action : actions) {
            actionEvents.emplace_back(ActionEvent {
                    .ActionName = action.actionName,
                    .Source = source,
                    .SourceIndex = deviceIndex,
                    .Value = newValue * action.scale,
                    .Context = input->_keyInputContext[key]
            });
        }
        return actionEvents;
    }

    void PropagateActionEvent (INPUT_MANAGER::IM inputManager, ActionEvent event) {
        for (size_t i = inputManager->_actionCallbacks[event.ActionName].size() - 1; i >= 0; i--) {
            auto& actionCallback = inputManager->_actionCallbacks[event.ActionName][i];

            if (actionCallback.Func(event.Source, event.SourceIndex, event.Value, event.Context)) break;
        }
    }

    void RegisterDevice (INPUT_MANAGER::IM inputManager, const InputDevice& device) {
        //DEBUG { spdlog::info ("Device registered of type: " + std::to_string(device.type));}
        DEBUG { spdlog::info ("Device registered of type: ", static_cast<int>(device.type));}
        inputManager->_devices.emplace_back(device);
    }

    void RemoveDevice (INPUT_MANAGER::IM inputManager, InputDeviceType type, int inputIndex) {
        erase_if(inputManager->_devices, [type, inputIndex](const InputDevice& device){
            DEBUG { spdlog::info ("Device unregistered of type: ", static_cast<int>(type));}
            return device.type == type && device.Index == inputIndex;
        });
    }

}

