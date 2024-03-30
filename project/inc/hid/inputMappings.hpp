//
// Created by Bartek on 26.03.2024.
//

#ifndef EVILENGINE_INPUTMAPPINGS_HPP
#define EVILENGINE_INPUTMAPPINGS_HPP

#endif //EVILENGINE_INPUTMAPPINGS_HPP

#include "global.hpp"

namespace INPUT_MAP {
    void HandleClick(float value) {
        std::cout << "Left mouse button: " << value << "\n";
    }

    void MapInputs(INPUT_MANAGER::IM inputManager) {
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_A, InputAction("moveX", -1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_D, InputAction("moveX", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_LEFT, InputAction("click"));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_L_THUMB_X, InputAction("moveX", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_L_THUMB_Y, InputAction("moveY", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_WEST, InputAction("click"));

    }

    void RegisterCallbacks(INPUT_MANAGER::IM inputManager) {
        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveX", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value) {
                    std::string direction{"NONE"};
                    if (value > 0.1f) direction = "RIGHT";
                    if (value < -0.1f) direction = "LEFT";
                    if (abs(value) > 0.1) DEBUG {spdlog::info("x: {0}", direction);}
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveY", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value) {
                    std::string direction{"NONE"};
                    if (value > 0.f) direction = "DOWN";
                    if (value < 0.f) direction = "UP";
                    if (abs(value) > 0.1) DEBUG {spdlog::info("y: {0}", direction);}
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "click", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value) {
                    HandleClick(value);
                    return true;
                }
        });
    }
}
