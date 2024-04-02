#pragma once

#include "types.hpp"
#include "global.hpp"

namespace WIN {

    static void glfwErrorCallback(int error, const char* description) {
        spdlog::error ("Glfw Error {0}: {1}\n", error, description);
    }

    void Initialize (const GLFWerrorfun& errorCallback) {
        glfwSetErrorCallback (errorCallback);
        if (!glfwInit()) throw;
    };

    void SetVersion() {
        #if defined(IMGUI_IMPL_OPENGL_ES2) 										// GLES 2.0 + GLSL 100
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);						//
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);						//
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);				//
		#elif defined(__APPLE__) 												// GLES 3.2 + GLSL 150
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);						//
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);						//
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  	// 3.2+ only
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            	// Required on Mac
		#else 																	// GLES 3.0 + GLSL 130
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);						//
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);						//
			//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// 3.2+ only
			//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);          	// 3.0+ only
		#endif
    }

    void Create (Window& window) {

		Initialize (&glfwErrorCallback);
        SetVersion ();

        window = glfwCreateWindow (GLOBAL::windowTransform[2], GLOBAL::windowTransform[3], "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);

        glfwSetWindowUserPointer(window, &GLOBAL::input);

        for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++) {
            glfwSetJoystickUserPointer(i, &GLOBAL::mainWindow);
            if (glfwJoystickPresent(i)) {
                DEBUG { spdlog::info ("Joystick ", i, " is present");}

                if (GLOBAL::inputManager) {
                    INPUT_MANAGER::RegisterDevice(GLOBAL::inputManager, InputDevice {
                            .type = InputDeviceType::GAMEPAD,
                            .Index = i,
                            .StateFunc = std::bind(HID_INPUT::GetGamepadStateById, std::ref(GLOBAL::input), std::placeholders::_1)
                    });
                }
            }
        }

        glfwSetJoystickCallback([](int joystickId, int event) {
// TODO: fix input error when connecting gamepad during run
            if (GLOBAL::inputManager) {
                //auto* input = dynamic_cast<HID_INPUT::Input*>(GLOBAL::mainWindow);
                auto* input = static_cast<HID_INPUT::Input*>(glfwGetJoystickUserPointer(joystickId));
                DEBUG { spdlog::info (glfwJoystickIsGamepad(joystickId));}
                if (input) {DEBUG { spdlog::info ("input is not null");}}
                if (input) {
                    DEBUG { spdlog::info (event);}
                    DEBUG { spdlog::info (glfwJoystickIsGamepad(joystickId));}
                    if (event == GLFW_CONNECTED && glfwJoystickIsGamepad(joystickId)) {
                        INPUT_MANAGER::RegisterDevice(GLOBAL::inputManager, InputDevice {
                                .type = InputDeviceType::GAMEPAD,
                                .Index = joystickId,
                                .StateFunc = std::bind(HID_INPUT::GetGamepadStateById, std::ref(GLOBAL::input), std::placeholders::_1)
                        });
                        DEBUG { spdlog::info ("Device connected");}
                    } else if (event == GLFW_DISCONNECTED) {
                        INPUT_MANAGER::RemoveDevice(GLOBAL::inputManager, InputDeviceType::GAMEPAD, joystickId);
                        DEBUG { spdlog::info ("Device disconnected");}
                    }
                }
            }
        });

        glfwSetKeyCallback(window, [](GLFWwindow* _window, int key, int scancode, int action, int mods){
            auto* input = static_cast<HID_INPUT::Input*>(glfwGetWindowUserPointer(_window));
            if (input) {


                float value = 0.f;

                switch (action) {
                    case GLFW_PRESS:
                        value = 1.f;
                        GLOBAL::input->_keyInputContext[HID_INPUT::KeyToInputKey(*input, key)] = InputContext::STARTED;
                        break;
                    case GLFW_REPEAT:
                        value = 1.f;
                        GLOBAL::input->_keyInputContext[HID_INPUT::KeyToInputKey(*input, key)] = InputContext::REPEATED;
                        break;
                    default:
                        GLOBAL::input->_keyInputContext[HID_INPUT::KeyToInputKey(*input, key)] = InputContext::CANCELED;
                        value = 0.f;
                }

                HID_INPUT::UpdateKeyboardState(*input, key, value);
            }
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* _window, int button, int action, int mods) {
            auto* input = static_cast<HID_INPUT::Input*>(glfwGetWindowUserPointer(_window));

            float value = 0.f;
            switch (action) {
                case GLFW_PRESS:
                    value = 1.f;
                    if (GLOBAL::input->_keyInputContext[HID_INPUT::MouseButtonToInputKey(*input, button)] != InputContext::CANCELED)
                    {
                        GLOBAL::input->_keyInputContext[HID_INPUT::MouseButtonToInputKey(*input, button)] = InputContext::REPEATED;
                    }
                    else
                    {
                        GLOBAL::input->_keyInputContext[HID_INPUT::MouseButtonToInputKey(*input, button)] = InputContext::STARTED;
                    }

                    break;
                case GLFW_REPEAT:
                    value = 1.f;
                    GLOBAL::input->_keyInputContext[HID_INPUT::MouseButtonToInputKey(*input, button)] = InputContext::REPEATED;
                    break;
                default:
                    GLOBAL::input->_keyInputContext[HID_INPUT::MouseButtonToInputKey(*input, button)] = InputContext::CANCELED;
                    value = 0.f;
            }

            if (input) {
                HID_INPUT::UpdateMouseState(*input, button, value);
            }
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* _window, double xPos, double yPos) {
            auto* input = static_cast<HID_INPUT::Input*>(glfwGetWindowUserPointer(_window));

            if (input) {
                HID_INPUT::UpdateMouseCursorState(*input, float(xPos), float(yPos));
            }
        });


        auto* inputManager = GLOBAL::inputManager;
        INPUT_MANAGER::RegisterDevice(inputManager, InputDevice{
                .type = InputDeviceType::KEYBOARD,
                .Index = 0,
                .StateFunc = std::bind(&HID_INPUT::GetKeyboardState, std::ref(GLOBAL::input), std::placeholders::_1)
        });

        INPUT_MANAGER::RegisterDevice(inputManager, InputDevice {
                .type = InputDeviceType::MOUSE,
                .Index = 0,
                .StateFunc = std::bind(&HID_INPUT::GetMouseState, std::ref(GLOBAL::input), std::placeholders::_1)
        });

        if (window == NULL) throw;
        glfwMakeContextCurrent (window);
        glfwSwapInterval (1); // Enable vsync

		//Initialize GLAD 
		gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress);

		DEBUG { spdlog::info ("Window succesfully Created"); }
        DEBUG { IMGUI::Create (window); }

        GLOBAL::Initialize ();
    }

    void Destroy (const Window& window) {
        glfwDestroyWindow (window);
		DEBUG { IMGUI::Destroy (); }
        glfwTerminate ();
    }
}