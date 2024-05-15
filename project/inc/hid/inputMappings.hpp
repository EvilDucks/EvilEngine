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

    int FindPlayerIndexByInputSource(InputSource source, int sourceIndex)
    {
        u64 deviceIndex = 0;
        INPUT_MANAGER::FindDevice(GLOBAL::inputManager, source, sourceIndex, deviceIndex);
        return GLOBAL::inputManager->_devices[deviceIndex].PlayerIndex;
    }

    void MapInputs(INPUT_MANAGER::IM inputManager) {
        // PLAYER MOVEMENT
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_A, InputAction("moveX", -1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_D, InputAction("moveX", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_W, InputAction("moveY", -1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_S, InputAction("moveY", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_L_THUMB_X, InputAction("moveX", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_L_THUMB_Y, InputAction("moveY", 1.f));

        // TESTING INPUTS
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_LEFT, InputAction("click"));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_WEST, InputAction("click"));

        // CAMERA CONTROL
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_POS_X, InputAction("moveCameraX", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_POS_Y, InputAction("moveCameraY", 1.f));

        // Track mouse position
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_POS_X, InputAction("UpdateMouseX", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_POS_Y, InputAction("UpdateMouseY", 1.f));

        // EDIT MODE CAMERA
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_W, InputAction("editCameraForwardPos", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_S, InputAction("editCameraForwardPos", -1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_A, InputAction("editCameraRightPos", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_D, InputAction("editCameraRightPos", -1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_PAGE_UP, InputAction("editCameraUpPos", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_PAGE_DOWN, InputAction("editCameraUpPos", -1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_UP, InputAction("editCameraPitch", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_DOWN, InputAction("editCameraPitch", -1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_LEFT, InputAction("editCameraYaw", -1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_RIGHT, InputAction("editCameraYaw", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_Q, InputAction("testRotation", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_E, InputAction("testRotation", -1.f));

        // TEST LIGHT
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_P, InputAction("lightX", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_O, InputAction("lightX", -1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_U, InputAction("lightY", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_I, InputAction("lightY", -1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_T, InputAction("lightZ", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_Y, InputAction("lightZ", -1.f));

        // DEBUG MODES
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_F1, InputAction("PlayMode", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_F2, InputAction("EditMode", 1.f));

        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_KP_ADD, InputAction("ChangeObject", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_KP_SUBTRACT, InputAction("ChangeObject", -1.f));
    }

    void RegisterCallbacks(INPUT_MANAGER::IM inputManager) {
        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "click", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if (GLOBAL::uiManager->currentHoverIndex > -1)
                    {
                        int playerIndex = FindPlayerIndexByInputSource(source, sourceIndex);
                        if ( playerIndex > -1)
                        {
                            switch(GLOBAL::uiManager->currentHoverType){
                                case UI::ElementType::BUTTON:
                                    UI::MANAGER::PropagateUIEvent(GLOBAL::uiManager, UI::MANAGER::UIEvent(GLOBAL::uiManager->buttons[GLOBAL::uiManager->currentHoverIndex].local.name, playerIndex, UI::ElementType::BUTTON));
                            }
                        }
                    }
                    return true;
                }
        });

#pragma region PLAY_MODE
        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveX", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(GLOBAL::mode == EDITOR::PLAY_MODE)
                    {
                        std::string direction{"NONE"};
                        if (value > 0.1f) direction = "RIGHT";
                        if (value < -0.1f) direction = "LEFT";
                        if (abs(value) > 0.1)
                        {
                            //DEBUG {spdlog::info("x: {0}", direction);}
                            u64 deviceIndex = 0;
                            INPUT_MANAGER::FindDevice(GLOBAL::inputManager, source, sourceIndex, deviceIndex);
                            if (GLOBAL::inputManager->_devices[deviceIndex].PlayerIndex >= 0)
                            {
                                PLAYER::PlayerMovementX(GLOBAL::players[GLOBAL::inputManager->_devices[deviceIndex].PlayerIndex], value, context);
                            }
                        }
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveY", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(GLOBAL::mode == EDITOR::PLAY_MODE)
                    {
                        std::string direction{"NONE"};
                        if (value > 0.f) direction = "DOWN";
                        if (value < 0.f) direction = "UP";
                        if (abs(value) > 0.1)
                        {
                            //DEBUG {spdlog::info("y: {0}", direction);}
                            u64 deviceIndex = 0;
                            INPUT_MANAGER::FindDevice(GLOBAL::inputManager, source, sourceIndex, deviceIndex);
                            if (GLOBAL::inputManager->_devices[deviceIndex].PlayerIndex >= 0)
                            {
                                PLAYER::PlayerMovementY(GLOBAL::players[GLOBAL::inputManager->_devices[deviceIndex].PlayerIndex], value, context);
                            }
                        }
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveCameraX", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    std::string direction{"NONE"};
                    if(GLOBAL::mode == EDITOR::PLAY_MODE)
                    {
                        if (value > 0.1f) direction = "RIGHT";
                        if (value < -0.1f) direction = "LEFT";
                        //if (abs(value) > 0.1) DEBUG {spdlog::info("x: {0}", direction);}
                        float xoffset = value - GLOBAL::lastX;
                        GLOBAL::lastX = value;
                        ProcessMouseMovementX(GLOBAL::world.camera, xoffset);
                        //DEBUG {spdlog::info("mouse x: {0}", value);}
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveCameraY", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    std::string direction{"NONE"};
                    if(GLOBAL::mode == EDITOR::PLAY_MODE)
                    {
                        if (value > 0.f) direction = "DOWN";
                        if (value < 0.f) direction = "UP";
                        //if (abs(value) > 0.1) DEBUG {spdlog::info("y: {0}", direction);}
                        float yoffset = value - GLOBAL::lastY;
                        GLOBAL::lastY = value;
                        ProcessMouseMovementY(GLOBAL::world.camera, yoffset);
                        //DEBUG {spdlog::info("mouse y: {0}", value);}
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "UpdateMouseX", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    DEBUG {spdlog::info("mouse x: {0}", value);}
                    int playerIndex = FindPlayerIndexByInputSource(source, sourceIndex);
                    if (playerIndex > -1)
                    {
                        GLOBAL::players[playerIndex].local.selection.x = value;
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "UpdateMouseY", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    DEBUG {spdlog::info("mouse y: {0}", value);}
                    int playerIndex = FindPlayerIndexByInputSource(source, sourceIndex);
                    if (playerIndex > -1)
                    {
                        GLOBAL::players[playerIndex].local.selection.y = value;
                    }
                    return true;
                }
        });

#pragma endregion PLAY_MODE

#pragma region EDIT_MODE
        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "editCameraYaw", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {

                    if(GLOBAL::mode == EDITOR::EDIT_MODE) {
                        GLOBAL::world.camera.local.yaw += value;
                        updateCameraVectors(GLOBAL::world.camera);
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "editCameraPitch", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(GLOBAL::mode == EDITOR::EDIT_MODE) {
                        GLOBAL::world.camera.local.pitch += value;
                        updateCameraVectors(GLOBAL::world.camera);
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "editCameraForwardPos", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(GLOBAL::mode == EDITOR::EDIT_MODE) {
                        float deltaTime = 0.1f;
                        if(value == 1)
                        {
                            processKeyBoard(GLOBAL::world.camera, CAMERA::Camera_Movement::FORWARD, deltaTime);
                        }
                        else if(value == -1)
                        {
                            processKeyBoard(GLOBAL::world.camera, CAMERA::Camera_Movement::BACKWARD, deltaTime);
                        }
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "editCameraRightPos", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(GLOBAL::mode == EDITOR::EDIT_MODE) {
                        float deltaTime = 0.1f;
                        if(value == 1)
                        {
                            processKeyBoard(GLOBAL::world.camera, CAMERA::Camera_Movement::LEFT, deltaTime);
                        }
                        else if(value == -1)
                        {
                            processKeyBoard(GLOBAL::world.camera, CAMERA::Camera_Movement::RIGHT, deltaTime);
                        }
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "editCameraUpPos", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    if(GLOBAL::mode == EDITOR::EDIT_MODE) {
                        float deltaTime = 0.1f;
                        if(value == 1)
                        {
                            processKeyBoard(GLOBAL::world.camera, CAMERA::Camera_Movement::UP, deltaTime);
                        }
                        else if(value == -1)
                        {
                            processKeyBoard(GLOBAL::world.camera, CAMERA::Camera_Movement::DOWN, deltaTime);
                        }
                    }
                    return true;
                }
        });
#pragma endregion EDIT_MODE

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "testRotation", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    std::string direction{"NONE"};
                    {
                        //DEBUG {spdlog::info("y: {0}", direction);}
                        u64 deviceIndex = 0;
                        INPUT_MANAGER::FindDevice(GLOBAL::inputManager, source, sourceIndex, deviceIndex);
                        if (GLOBAL::inputManager->_devices[deviceIndex].PlayerIndex >= 0)
                        {
                            PLAYER::PlayerRotation(GLOBAL::players[GLOBAL::inputManager->_devices[deviceIndex].PlayerIndex], value, context);
                        }
                    }
                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "lightX", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {

                    GLOBAL::lightPosition.x += value/ 100;

                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "lightY", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {

                    GLOBAL::lightPosition.y += value/ 100;

                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "lightZ", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {

                    GLOBAL::lightPosition.z += value / 100;

                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "PlayMode", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {

                    DEBUG
                    {
                        if (context == InputContext::STARTED)
                        {
                            spdlog::info ("Play mode");
                            GLOBAL::mode = EDITOR::PLAY_MODE;
                        }
                    }

                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "EditMode", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {

                    DEBUG
                    {
                        if (context == InputContext::STARTED)
                        {
                            spdlog::info ("Edit mode");
                            GLOBAL::mode = EDITOR::EDIT_MODE;
                        }
                    }

                    return true;
                }
        });

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "ChangeObject", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {

                    DEBUG
                    {
                        if (GLOBAL::mode == EDITOR::EDIT_MODE)
                        {
                            if (context == InputContext::STARTED)
                            {
                                //spdlog::info ("Transform {0}", G);
                                GLOBAL::editedObject += value;
                                if (GLOBAL::editedObject > GLOBAL::world.transformsCount)
                                {
                                    GLOBAL::editedObject = 0;
                                }
                            }
                        }

                    }

                    return true;
                }
        });
    }
}

