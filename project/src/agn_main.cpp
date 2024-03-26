// Made by Matthew Strumillo 2024-03-08
//

#include "platform/agn/window.hpp"
#include "render/render.hpp"
#include "hid/inputMappings.hpp"

int main() {

	DEBUG { spdlog::info ("Entered Agnostic-x86_64-Platform execution."); }

    HID_INPUT::Create(GLOBAL::input);
    INPUT_MANAGER::Create (GLOBAL::inputManager);
	WIN::Create (GLOBAL::mainWindow);

    if (GLOBAL::inputManager) {
        INPUT_MAP::MapInputs(GLOBAL::inputManager);
        INPUT_MAP::RegisterCallbacks(GLOBAL::inputManager);
    }

	while (!glfwWindowShouldClose (GLOBAL::mainWindow)) {
        if (GLOBAL::inputManager) {
            INPUT_MANAGER::ProcessInput(GLOBAL::inputManager);
        }

        glfwPollEvents ();
		
		glfwGetFramebufferSize (
			GLOBAL::mainWindow, 
			&GLOBAL::windowTransform[2], 
			&GLOBAL::windowTransform[3]
		);

        RENDER::Render ();
	}

	DEBUG { spdlog::info ("Finishing execution."); }
	GLOBAL::Destroy();
	WIN::Destroy(GLOBAL::mainWindow);
    INPUT_MANAGER::Destroy(GLOBAL::inputManager);
    HID_INPUT::Destroy(GLOBAL::input);
	DEBUG { spdlog::info ("Closing Program."); }

	return 0;
}