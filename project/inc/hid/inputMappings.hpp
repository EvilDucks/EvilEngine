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
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::A, InputAction("move", -1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::D, InputAction("move", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_LEFT, InputAction("click"));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_LEFT, InputAction("click", .01f));

        INPUT_MANAGER::UnmapInputFromAction(GLOBAL::inputManager, InputKey::MOUSE_LEFT, "click");

        INPUT_MANAGER::UnmapInputFromAction(GLOBAL::inputManager, InputKey::MOUSE_LEFT, "click");

    }

    void RegisterCallbacks(INPUT_MANAGER::IM inputManager) {
        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "move", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value) {
                    std::string direction{"NONE"};
                    if (value == 1.f) direction = "RIGHT";
                    if (value == -1.f) direction = "LEFT";

                    std::cout << "MOVING" << direction << "\n";
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

