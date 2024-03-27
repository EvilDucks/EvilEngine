//
// Created by Bartek on 22.03.2024.
//

#ifndef EVILENGINE_INPUTMANAGER_H
#define EVILENGINE_INPUTMANAGER_H

#endif //EVILENGINE_INPUTMANAGER_H

#include "inputKey.hpp"
#include "inputDevices.hpp"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <functional>


namespace INPUT_MANAGER {

    InputSource GetInputSourceFromKey (InputKey key) {
        switch (key) {
            case InputKey::A:
            case InputKey::B:
            case InputKey::C:
            case InputKey::D:
            case InputKey::E:
            case InputKey::S:
            case InputKey::W:
            case InputKey::SPACEBAR:
            case InputKey::LSHIFT:
                return InputSource::KEYBOARD;
            case InputKey::L_THUMB_X:
            case InputKey::L_THUMB_Y:
            case InputKey::R_THUMB_X:
            case InputKey::R_THUMB_Y:
                return InputSource::GAMEPAD;
            case InputKey::MOUSE_LEFT:
            case InputKey::MOUSE_MIDDLE:
            case InputKey::MOUSE_RIGHT:
            case InputKey::MOUSE_MOVE_X:
            case InputKey::MOUSE_MOVE_Y:
                return InputSource::MOUSE;
            default:
                return InputSource::UNKNOWN;

        }
    }

    using ActionCallbackFunc = std::function<bool(InputSource, int, float)>;

    struct ActionCallback {
        std::string Ref;
        ActionCallbackFunc Func;
    };

    struct ActionEvent {
        std::string ActionName;
        InputSource Source;
        int SourceIndex;
        float Value;
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
    void ProcessInput (INPUT_MANAGER::IM inputManager);
    std::vector<ActionEvent> GenerateActionEvent (INPUT_MANAGER::IM inputManager, int deviceIndex, InputKey key, float newValue);
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

    void ProcessInput (INPUT_MANAGER::IM inputManager) {
        std::vector<ActionEvent> events {};
        for (auto& device : inputManager->_devices) {
            auto newState = device.StateFunc(device.Index);

            for (auto& keyState : newState) {
                if (device.CurrentState[keyState.first].value != keyState.second.value) {
                    // TODO: check for conflicts
                    auto generatedEvents = GenerateActionEvent(inputManager, device.Index, keyState.first, keyState.second.value);
                    events.insert(events.end(), generatedEvents.begin(), generatedEvents.end());

                    device.CurrentState[keyState.first].value = keyState.second.value;
                }
            }
        }
        for (auto& event : events) {
            PropagateActionEvent(inputManager, event);
        }

    }

    std::vector<ActionEvent> GenerateActionEvent (INPUT_MANAGER::IM inputManager, int deviceIndex, InputKey key, float newValue) {
        auto& actions = inputManager->_inputActionMapping[key];

        std::vector<ActionEvent> actionEvents {};

        InputSource source = INPUT_MANAGER::GetInputSourceFromKey(key);

        for (auto& action : actions) {
            actionEvents.emplace_back(ActionEvent {
                    .ActionName = action.actionName,
                    .Source = source,
                    .SourceIndex = deviceIndex,
                    .Value = newValue * action.scale
            });
        }
        return actionEvents;
    }

    void PropagateActionEvent (INPUT_MANAGER::IM inputManager, ActionEvent event) {
        for (size_t i = inputManager->_actionCallbacks[event.ActionName].size() - 1; i >= 0; i--) {
            auto& actionCallback = inputManager->_actionCallbacks[event.ActionName][i];

            if (actionCallback.Func(event.Source, event.SourceIndex, event.Value)) break;
        }
    }

    void RegisterDevice (INPUT_MANAGER::IM inputManager, const InputDevice& device) {
        inputManager->_devices.emplace_back(device);
    }

    void RemoveDevice (INPUT_MANAGER::IM inputManager, InputDeviceType type, int inputIndex) {
        erase_if(inputManager->_devices, [type, inputIndex](const InputDevice& device){
            return device.type == type && device.Index == inputIndex;
        });
    }

}

