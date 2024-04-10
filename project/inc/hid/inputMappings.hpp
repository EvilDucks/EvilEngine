//
// Created by Bartek on 26.03.2024.
//

#ifndef EVILENGINE_INPUTMAPPINGS_HPP
#define EVILENGINE_INPUTMAPPINGS_HPP

#endif //EVILENGINE_INPUTMAPPINGS_HPP

#include "global.hpp"

namespace INPUT_MAP {
    void HandleClick(float value, InputContext context) {
        if (context == InputContext::STARTED)
        {
            DEBUG {spdlog::info("Click: {0}", value);}
        }
//        switch (context)
//        {
//            case InputContext::STARTED:
//                DEBUG {spdlog::info("Click STARTED");}
//                break;
//            case InputContext::REPEATED:
//                DEBUG {spdlog::info("Click REPEATED");}
//                break;
//            case InputContext::CANCELED:
//                DEBUG {spdlog::info("Click CANCELED");}
//        }
    }

    void MapInputs(INPUT_MANAGER::IM inputManager) {
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_A, InputAction("moveX", -1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_D, InputAction("moveX", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_W, InputAction("moveY", -1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_S, InputAction("moveY", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_LEFT, InputAction("click"));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_L_THUMB_X, InputAction("moveX", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_L_THUMB_Y, InputAction("moveY", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_WEST, InputAction("click"));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_POS_X, InputAction("moveCameraX", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_POS_Y, InputAction("moveCameraY", 1.f));

        // TEST CAMERA SECTION
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_B, InputAction("testCamera1", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_C, InputAction("testCamera2", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_PAGE_UP, InputAction("testCameraZoom1", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_PAGE_DOWN, InputAction("testCameraZoom2", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_1, InputAction("testCameraYaw1", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_2, InputAction("testCameraYaw2", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_3, InputAction("testCameraPitch1", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_4, InputAction("testCameraPitch2", 1.f));
    }

    void RegisterCallbacks(INPUT_MANAGER::IM inputManager) {
        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveX", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    std::string direction{"NONE"};
                    if (value > 0.1f) direction = "RIGHT";
                    if (value < -0.1f) direction = "LEFT";
                    if (abs(value) > 0.1)
                    {
                        //DEBUG {spdlog::info("x: {0}", direction);}
                        u64 deviceIndex = 0;
                        INPUT_MANAGER::FindDevice(GLOBAL::inputManager, source, sourceIndex, deviceIndex);
                        PLAYER::PlayerMovementX(GLOBAL::players[GLOBAL::inputManager->_devices[deviceIndex].playerIndex], value, context);
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveY", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    std::string direction{"NONE"};
                    if (value > 0.f) direction = "DOWN";
                    if (value < 0.f) direction = "UP";
                    if (abs(value) > 0.1)
                    {
                        //DEBUG {spdlog::info("y: {0}", direction);}
                        u64 deviceIndex = 0;
                        INPUT_MANAGER::FindDevice(GLOBAL::inputManager, source, sourceIndex, deviceIndex);
                        PLAYER::PlayerMovementY(GLOBAL::players[GLOBAL::inputManager->_devices[deviceIndex].playerIndex], value, context);
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "click", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    HandleClick(value, context);
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveCameraX", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    std::string direction{"NONE"};
                    if (value > 0.1f) direction = "RIGHT";
                    if (value < -0.1f) direction = "LEFT";
                    //if (abs(value) > 0.1) DEBUG {spdlog::info("x: {0}", direction);}
                    float xoffset = value - GLOBAL::lastX;
                    GLOBAL::lastX = value;
                    ProcessMouseMovementX(GLOBAL::world.camera, xoffset);
                    //DEBUG {spdlog::info("mouse x: {0}", value);}
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveCameraY", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    std::string direction{"NONE"};
                    if (value > 0.f) direction = "DOWN";
                    if (value < 0.f) direction = "UP";
                    //if (abs(value) > 0.1) DEBUG {spdlog::info("y: {0}", direction);}
                    float yoffset = value - GLOBAL::lastY;
                    GLOBAL::lastY = value;
                    ProcessMouseMovementY(GLOBAL::world.camera, yoffset);
                    //DEBUG {spdlog::info("mouse y: {0}", value);}
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "testCamera1", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(value == 1)
                    {
                        GLOBAL::world.camera.local.position.x += 0.05f;
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "testCamera2", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(value == 1)
                    {
                        GLOBAL::world.camera.local.position.x -= 0.05f;
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "testCameraZoom1", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(value == 1)
                    {
                        GLOBAL::world.camera.local.zoom += 0.5f;
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "testCameraZoom2", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(value == 1)
                    {
                        GLOBAL::world.camera.local.zoom -= 0.5f;
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "testCameraYaw1", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(value == 1)
                    {
                        GLOBAL::world.camera.local.yaw -= 1.0f;
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "testCameraYaw1", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(value == 1)
                    {
                        GLOBAL::world.camera.local.yaw -= 1.0f;
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "testCameraYaw2", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(value == 1)
                    {
                        GLOBAL::world.camera.local.yaw += 1.0f;
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "testCameraPitch1", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(value == 1)
                    {
                        GLOBAL::world.camera.local.pitch -= 1.0f;
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "testCameraPitch2", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(value == 1)
                    {
                        GLOBAL::world.camera.local.pitch += 1.0f;
                    }
                    return true;
                }
        });
    }
}

