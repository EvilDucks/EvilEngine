// Made by Matthew Strumillo 2024-02-21
//

//#define WINDOW_ALPHA
//#define WINDOW_CUSTOM
//#define WINDOW_CUSTOM_WITH_SHADOW
#include "platform/windows.hpp"

#include "platform/console.hpp"
#include "types.hpp"
#include "debug.hpp"
#include "global.hpp"

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
	HWND window = nullptr;

	WIN::Create (instance, window, windowName, GLOBAL::windowSize);
	
    MSG msg { 0 }; // Main loop
    while (msg.message != WM_QUIT) {

		if (PeekMessage (&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
            continue;
        } 

		WIN::Render ();
		
	}

	WIN::Destroy (instance, window, windowName);

	return 0;
}