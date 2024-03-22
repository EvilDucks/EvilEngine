// Made by Matthew Strumillo 2024-03-08
//

#include "platform/agn/window.hpp"
#include "render/render.hpp"

int main() {

	DEBUG { spdlog::info ("Entered Agnostic-x86_64-Platform execution."); }

	WIN::Create (GLOBAL::mainWindow);

	while (!glfwWindowShouldClose (GLOBAL::mainWindow)) {
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
	DEBUG { spdlog::info ("Closing Program."); }

	return 0;
}