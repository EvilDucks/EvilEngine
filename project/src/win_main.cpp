// Made by Matthew Strumillo 2024-02-21
//

#define WINDOW_ALPHA
//#define WINDOW_CUSTOM
//#define WINDOW_CUSTOM_WITH_SHADOW

#include "platform/win/console.hpp"
#include "platform/win/window.hpp"
#include "render/render.hpp"

s32 WinMain (
	HINSTANCE	instance,
	HINSTANCE	previousInstance,
	c8*			arguments,
	s32			windowState
) {
	UNREFERENCED_PARAMETER (previousInstance);
	UNREFERENCED_PARAMETER (arguments);
	UNREFERENCED_PARAMETER (windowState);

	DEBUG { 
		AttachConsole (); // Create a new console host.
		spdlog::info ("Entered Windowns-x86_64-Platform execution."); 
	}

	c16 windowName[] { L"omw" };

	WIN::Create (instance, GLOBAL::mainWindow, windowName, GLOBAL::windowTransform);

	MSG msg { 0 }; // Main loop
	while (msg.message != WM_QUIT) {

		if (PeekMessage (&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage (&msg);
			DispatchMessage (&msg);
			continue;
		} 

		{ // glfwGetFramebufferSize alt.
    		GetClientRect (GLOBAL::mainWindow, &GLOBAL::windowTransform);
		}

		RENDER::Render ();
		
	}

	DEBUG { spdlog::info ("Finishing execution."); }
	GLOBAL::Destroy();
	WIN::Destroy (instance, GLOBAL::mainWindow, windowName);
	DEBUG { spdlog::info ("Closing Program."); }

	return 0;
}