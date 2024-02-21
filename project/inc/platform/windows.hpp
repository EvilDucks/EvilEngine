// Made by Matthew Strumillo 2024-02-21
//

#pragma once

#include <windowsx.h> // GET_X_LPARAM, and such
#include <dwmapi.h> // blur-alpha things

#include "loader.hpp"
#include "types.hpp"
#include "debug.hpp"
#include "global.hpp"
#include "render.hpp"

namespace WIN {

	void WindowAlpha (
		HWND window
	) {
		// https://stackoverflow.com/questions/1617370/how-to-use-alpha-transparency-in-opengl#1617379
        // 

        DWM_BLURBEHIND bb { 0 };
        bb.fEnable = TRUE;
        HRGN hRgn = CreateRectRgn(0, 0, -1, -1); // invalid values so broken-areo-blur doesn't render...
        bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
        bb.hRgnBlur = hRgn;
        DwmEnableBlurBehindWindow(window, &bb);

		// then happedns
		// PIXELFORMATDESCRIPTOR, glEnable, glClearColor with alpha
	}

	void Render () {
		IMGUI::Render (GLOBAL::backgroundColor);
        wglMakeCurrent (LOADER::graphicalContext, LOADER::openGLRenderContext);
        RENDER::Update (GLOBAL::windowSize, GLOBAL::backgroundColor);
		IMGUI::PostRender ();
        //wglMakeCurrent (LOADER::graphicalContext, LOADER::openGLRenderContext);
		SwapBuffers (LOADER::graphicalContext);
	}

    s64 Procedure (
    	HWND window, 
    	UINT message, 
    	WPARAM wParam, 
    	LPARAM lParam
    ) {
        DEBUG { if (ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam)) return true; }

        switch (message) {

			case WM_CREATE: {

				WindowAlpha (window);

				LOADER::CreateGlContext (window);
				LOADER::SetSwapInterval (1);
				
				//MARGINS margins { 0, 0, 0, 0 };
				//MARGINS margins { 1, 1, 1, 1 };
				//DwmExtendFrameIntoClientArea (window, &margins);

				// OpenAL ALpha
				glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable (GL_BLEND);

				IMGUI::Create (window);

				DEBUG { spdlog::info ("Window succesfully Created"); }
			} break;

    		case WM_SIZE: {
				//DEBUG { spdlog::info ("Window Resized"); }
				Render();
			} break;

			case WM_MOVE: {
				//DEBUG { spdlog::info ("Window Moved!"); }
				Render();
			} break;

			case WM_MOUSEMOVE: {
				s32 xPos = GET_X_LPARAM(lParam); 
				s32 yPos = GET_Y_LPARAM(lParam);
				DEBUG { spdlog::info ("MousePos: {0}, {1}", xPos, yPos); }
			} break;

			case WM_NCCALCSIZE: {
				//DEBUG { spdlog::info ("Call"); }
			} break; //return 0;

			case WM_NCHITTEST: {
				//DEBUG { spdlog::info ("Call"); }
                // When we have no border or title bar, we need to perform our
                // own hit testing to allow resizing and moving.
                //if (window.borderless) {
                //    return window.hit_test(
				//		POINT {
                //    	    GET_X_LPARAM(lparam),
                //    	    GET_Y_LPARAM(lparam)
                //    	}
				//	);
                //}
                break;
            }

			//case WM_NCACTIVATE: {
            //    break;
            //}

    			//	if (wParam != SIZE_MINIMIZED) {
    			//		windowSize[0] = (UINT) LOWORD (lParam);
    			//		windowSize[1] = (UINT) HIWORD (lParam);
				//		return 0;
    			//	}
				//	break;

			//case WM_PAINT:
			//	DEBUG { spdlog::info ("Paint!"); }
			//	break;
    
    		case WM_SYSCOMMAND:
    			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
    				return 0;
    			break;
    
    		case WM_DESTROY:
    			PostQuitMessage (0);
    			return 0;
    	}

    	return DefWindowProcW (window, message, wParam, lParam);
    }

    void Create (
        HINSTANCE instance,
        HWND& window,
        c16* windowName,
        u16* windowSize
    ) {
        c16 windowTitle[] { L"EngineOne" };

	    u16 windowPosition[2] { 100, 100 };

	    WNDCLASSEXW windowClass { 0 };
	    windowClass.cbSize 			= sizeof(WNDCLASSEXW);
	    windowClass.lpfnWndProc		= Procedure;
	    windowClass.hInstance     	= instance;
	    windowClass.hIcon			= nullptr; // (HICON)LoadImage(process, RESOURCE_LOCATION TEXTURE_ICON_MAIN, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	    windowClass.hbrBackground 	= nullptr; // (HBRUSH)(COLOR_BACKGROUND);
		//windowClass.hbrBackground = (HBRUSH)CreateSolidBrush(0x00000000);
	    windowClass.lpszClassName 	= windowName;
	    windowClass.style 			= CS_OWNDC;

	    if (!RegisterClassExW (&windowClass) ) exit (1);

	    window = CreateWindowExW (
	    	WS_EX_LEFT,
	    	windowName,
	    	windowTitle,
	    	WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			//WS_POPUP | WS_VISIBLE,	// TO MAKE CUSTOM WINDOW 
			//WS_CAPTION | WS_VISIBLE,
			//WS_POPUP | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE,
			//WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE,
	    	windowPosition[0], windowPosition[1],
	    	windowSize[0], windowSize[1],
	    	0, 0,
	    	instance,
	    	0
	    );

		// NOW WM_CREATE TRIGGERS! 
		//  To compete with WM_PAINT WM_SIZE messages
		//  required CREATION logic is being moved over there!
		

		// Show the window
		//ShowWindow (window, SW_SHOWDEFAULT);
	    //UpdateWindow (window);

		//Prepare OpenGlContext
	    //LOADER::CreateGlContext(window);
	    //LOADER::SetSwapInterval(1);
		//IMGUI::Create (window);
	    
		//DEBUG { 
	    //	spdlog::info ("Successfully created window!"); 
		//}
    }

	// FOR SWAPPING !!!
	void DeleteContext() {
		wglMakeCurrent(LOADER::graphicalContext, NULL);
		wglDeleteContext(LOADER::openGLRenderContext);
	}

    void Destroy (
		HINSTANCE instance,
		HWND& window,
		c16* windowName
	) { 
		wglDeleteContext (WIN::LOADER::openGLRenderContext);
		IMGUI::Destroy();
		UnregisterClassW (windowName, instance);
	}
}