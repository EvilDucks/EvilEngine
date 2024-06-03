//
// Created by Bartek on 26.03.2024.
//

#ifndef EVILENGINE_INPUTMAPPINGS_HPP
#define EVILENGINE_INPUTMAPPINGS_HPP

#endif //EVILENGINE_INPUTMAPPINGS_HPP

#include "global.hpp"
#include "player/playerMovement.hpp"

namespace INPUT_MAP {

	int FindPlayerIndexByInputSource(InputSource source, int sourceIndex)
	{
		int deviceIndex = -1;
		INPUT_MANAGER::FindDevice(GLOBAL::inputManager, source, sourceIndex, deviceIndex);
		if (deviceIndex == -1)
		{
			return -1;
		}
		else
		{
			return GLOBAL::inputManager->_devices[deviceIndex].PlayerIndex;
		}
	}

	void MapInputs(INPUT_MANAGER::IM inputManager) {
		// PLAYER MOVEMENT
		INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_A, InputAction("moveX", -1.f));
		INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_D, InputAction("moveX", 1.f));
		INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_W, InputAction("moveY", -1.f));
		INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_S, InputAction("moveY", 1.f));
		INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_SPACE, InputAction("Jump", 1.f));
		INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_SOUTH, InputAction("Jump", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::KEYBOARD_LEFT_SHIFT, InputAction("Charge", 1.f));
        INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_RT, InputAction("Charge", 1.f));


		INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_L_THUMB_X, InputAction("moveX", 1.f));
		INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_L_THUMB_Y, InputAction("moveY", 1.f));

		// TESTING INPUTS
		INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_LEFT, InputAction("click"));
		INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_WEST, InputAction("click"));

		// CAMERA CONTROL
		INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::MOUSE_POS_X, InputAction("moveCameraX", 1.f));
		INPUT_MANAGER::MapInputToAction(GLOBAL::inputManager, InputKey::GAMEPAD_R_THUMB_X, InputAction("moveCameraX", 1.f));
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
					if (GLOBAL::uiManager->currentHoverIndex > -1 && context == InputContext::CANCELED)
					{
						int playerIndex = FindPlayerIndexByInputSource(source, sourceIndex);
						if ( playerIndex > -1)
						{
							UI::MANAGER::UIEvent event;
							switch(GLOBAL::uiManager->currentHoverType){
								case UI::ElementType::BUTTON:
									event.eventName = GLOBAL::canvas.buttons[GLOBAL::uiManager->currentHoverIndex].local.name;
									event.playerIndex = playerIndex;
									event.elementType = UI::ElementType::BUTTON;
									event.value = 2;
									UI::MANAGER::PropagateUIEvent(GLOBAL::uiManager, event);
									GLOBAL::canvas.buttons[GLOBAL::uiManager->currentHoverIndex].local.state = value;
									break;
								default:
									break;
							}
						}
					}

					if (context == InputContext::STARTED)
					{

                        // For debugging purposes, delete later
                        //PLAYER::MOVEMENT::MovementLock(GLOBAL::players[0], 5.0f);
                        //RIGIDBODY::AddForce(GLOBAL::world.rigidbodies[GLOBAL::players[FindPlayerIndexByInputSource(source, sourceIndex)].local.rigidbodyIndex], glm::vec3(5.f, 5.f, 0.f), 1.f, -1.f);
					}
					return true;
				}
		});

#pragma region PLAY_MODE

		INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveX", INPUT_MANAGER::ActionCallback{
				.Ref = "Game",
				.Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
					EDITOR_PLAY_MODE_OR_RELEASE_ONLY ({
						if (fabs(value) > 0.1f)
						{
							int playerIndex = FindPlayerIndexByInputSource(source, sourceIndex);
							if (playerIndex > -1)
							{
								PLAYER::MOVEMENT::Horizontal(GLOBAL::players[playerIndex], value, context);
							}
						}
						else
						{
							int playerIndex = FindPlayerIndexByInputSource(source, sourceIndex);
							if (playerIndex > -1)
							{
								PLAYER::MOVEMENT::Horizontal(GLOBAL::players[playerIndex], 0, context);
							}
						}
					})

					return true;
				}
		});

		INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveY", INPUT_MANAGER::ActionCallback{
				.Ref = "Game",
				.Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
					EDITOR_PLAY_MODE_OR_RELEASE_ONLY ({
						if (fabs(value) > 0.1f)
						{
							int playerIndex = FindPlayerIndexByInputSource(source, sourceIndex);
							if ( playerIndex > -1)
							{
								PLAYER::MOVEMENT::Vertical(GLOBAL::players[playerIndex], value, context);
							}
						}
						else
						{
							int playerIndex = FindPlayerIndexByInputSource(source, sourceIndex);
							if ( playerIndex > -1)
							{
								PLAYER::MOVEMENT::Vertical(GLOBAL::players[playerIndex], 0, context);
							}
						}
					})
					return true;
				}
		});

		INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "Jump", INPUT_MANAGER::ActionCallback{
				.Ref = "Game",
				.Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
					EDITOR_PLAY_MODE_OR_RELEASE_ONLY ({
						if(context == InputContext::STARTED)
						{
							int playerIndex = FindPlayerIndexByInputSource(source, sourceIndex);
							if ( playerIndex > -1)
							{
								PLAYER::MOVEMENT::Jump (GLOBAL::players[playerIndex], GLOBAL::world.rigidbodies);
							}
						}
					})
					return true;
				}
		});

        INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "Charge", INPUT_MANAGER::ActionCallback{
                .Ref = "Game",
                .Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
                    EDITOR_PLAY_MODE_OR_RELEASE_ONLY ({
                          if(context == InputContext::STARTED)
                          {
                              int playerIndex = FindPlayerIndexByInputSource(source, sourceIndex);
                              if ( playerIndex > -1)
                              {
                                  PLAYER::MOVEMENT::Charge (GLOBAL::players[playerIndex], GLOBAL::world.rigidbodies);
                              }
                          }
                    })
                    return true;
                }
        });


		INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveCameraX", INPUT_MANAGER::ActionCallback{
				.Ref = "Game",
				.Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
					std::string direction{"NONE"};
					EDITOR_PLAY_MODE_OR_RELEASE_ONLY ({
						int playerIndex = FindPlayerIndexByInputSource(source, sourceIndex);

						if (source == InputSource::GAMEPAD )
						{
							if ( playerIndex > -1)
							{
								if (fabs(value) > 0.1)
								{
									const u8 GAMEPAD_FIX_VALUE = 5;
									ProcessMouseMovementX(GLOBAL::viewports[playerIndex].camera, value * GAMEPAD_FIX_VALUE);
									PLAYER::MOVEMENT::ChangeDirection(GLOBAL::players[playerIndex], GLOBAL::viewports[playerIndex].camera.local.yaw);

								}
							}
						}

						if (source == InputSource::MOUSE)
						{
							float xoffset = value - GLOBAL::lastX;
							GLOBAL::lastX = value;
							if ( playerIndex > -1)
							{
								ProcessMouseMovementX(GLOBAL::viewports[playerIndex].camera, xoffset);
								PLAYER::MOVEMENT::ChangeDirection(GLOBAL::players[playerIndex], GLOBAL::viewports[playerIndex].camera.local.yaw);
							}
						}
					})
					return true;
				}
		});

		INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "moveCameraY", INPUT_MANAGER::ActionCallback{
				.Ref = "Game",
				.Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
					EDITOR_PLAY_MODE_OR_RELEASE_ONLY ({
						float yoffset = value - GLOBAL::lastY;
						GLOBAL::lastY = value;
						ProcessMouseMovementY(GLOBAL::viewports[0].camera, yoffset);
					})
					return true;
				}
		});

		INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "UpdateMouseX", INPUT_MANAGER::ActionCallback{
				.Ref = "Game",
				.Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
					//DEBUG {spdlog::info("mouse x: {0}", value);}
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
					//DEBUG {spdlog::info("mouse y: {0}", value);}
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
					if (EDITOR::mode == EDITOR::EDIT_MODE) {
						GLOBAL::viewports[0].camera.local.yaw += value;
						CAMERA::UpdateCameraVectors(GLOBAL::viewports[0].camera);
					}
					return true;
				}
		});

		INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "editCameraPitch", INPUT_MANAGER::ActionCallback{
				.Ref = "Game",
				.Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
					if (EDITOR::mode == EDITOR::EDIT_MODE) {
						GLOBAL::viewports[0].camera.local.pitch += value;
						CAMERA::UpdateCameraVectors(GLOBAL::viewports[0].camera);
					}
					return true;
				}
		});

		INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "editCameraForwardPos", INPUT_MANAGER::ActionCallback{
				.Ref = "Game",
				.Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
					if (EDITOR::mode == EDITOR::EDIT_MODE) {
						float deltaTime = 0.1f;
						if (value == 1)
						{
							CAMERA::ProcessKeyboard(GLOBAL::viewports[0].camera, CAMERA::CameraMovement::FORWARD, deltaTime);
						}
						else if (value == -1)
						{
							CAMERA::ProcessKeyboard(GLOBAL::viewports[0].camera, CAMERA::CameraMovement::BACKWARD, deltaTime);
						}
					}
					return true;
				}
		});

		INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "editCameraRightPos", INPUT_MANAGER::ActionCallback{
				.Ref = "Game",
				.Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
					if (EDITOR::mode == EDITOR::EDIT_MODE) {
						float deltaTime = 0.1f;
						if (value == 1)
						{
							CAMERA::ProcessKeyboard(GLOBAL::viewports[0].camera, CAMERA::CameraMovement::LEFT, deltaTime);
						}
						else if (value == -1)
						{
							CAMERA::ProcessKeyboard(GLOBAL::viewports[0].camera, CAMERA::CameraMovement::RIGHT, deltaTime);
						}
					}
					return true;
				}
		});

		INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "editCameraUpPos", INPUT_MANAGER::ActionCallback{
				.Ref = "Game",
				.Func = [](InputSource source, int sourceIndex, float value, InputContext context) {
					if (EDITOR::mode == EDITOR::EDIT_MODE) {
						float deltaTime = 0.1f;
						if (value == 1)
						{
							CAMERA::ProcessKeyboard(GLOBAL::viewports[0].camera, CAMERA::CameraMovement::UP, deltaTime);
						}
						else if (value == -1)
						{
							CAMERA::ProcessKeyboard(GLOBAL::viewports[0].camera, CAMERA::CameraMovement::DOWN, deltaTime);
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
						int playerIndex = FindPlayerIndexByInputSource(source, sourceIndex);
						if ( playerIndex > -1)
						{
							PLAYER::PlayerRotation(GLOBAL::players[playerIndex], value, context, GLOBAL::world.lTransforms, GLOBAL::world.gTransforms, GLOBAL::world.colliders);
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
							EDITOR::mode = EDITOR::PLAY_MODE;
							GLOBAL::viewportsCount = 2;
							for(int i = 0; i < GLOBAL::playerCount; i++) {
								if(GLOBAL::inputManager->_devices[i].type == InputDeviceType::MOUSE)
								{
									glfwSetInputMode(GLOBAL::mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
									GLOBAL::viewports[0].camera.type = CAMERA::CameraType::THIRD_PERSON;
									GLOBAL::viewports[1].camera.type = CAMERA::CameraType::THIRD_PERSON;
									break;
								}
							}
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
							EDITOR::mode = EDITOR::EDIT_MODE;
							GLOBAL::viewportsCount = 1;
							for (int i = 0; i < GLOBAL::playerCount; i++) {
								if (GLOBAL::inputManager->_devices[i].type == InputDeviceType::MOUSE)
								{
									glfwSetInputMode(GLOBAL::mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
									GLOBAL::viewports[0].camera.type = CAMERA::CameraType::FREE;
									GLOBAL::viewports[1].camera.type = CAMERA::CameraType::FREE;
									break;
								}
							}
						}
					}

					return true;
				}
		});

		INPUT_MANAGER::RegisterActionCallback(GLOBAL::inputManager, "ChangeObject", INPUT_MANAGER::ActionCallback{
				.Ref = "Game",
				.Func = [](InputSource source, int sourceIndex, float value, InputContext context) {

					// RELEASE BREAKS 
					// 1. move all editor only callbacks to some other header file
					// 2. include these only when in DEBUG #ifdef XYZ #include ZYX #endif

					//DEBUG
					//{
					//	if (EDITOR::mode == EDITOR::EDIT_MODE)
					//	{
					//		if (context == InputContext::STARTED)
					//		{
					//			//spdlog::info ("Transform {0}", G);
					//			GLOBAL::EDITOR::editedObject += value;
					//			if (GLOBAL::EDITOR::editedObject > GLOBAL::world.transformsCount)
					//			{
					//				GLOBAL::EDITOR::editedObject = 0;
					//			}
					//		}
					//	}
					//}

					return true;
				}
		});
	}
}

