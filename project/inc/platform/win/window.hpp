// Made by Matthew Strumillo 2024-02-21
//

// more on borderless windows
// https://github.com/melak47/BorderlessWindow/blob/main/src/BorderlessWindow.cpp#L158
//


// change from WM_MOVE, WM_SIZE to SIZING AND MOVING TO HAVE ACTIVE RENDERING WHILE MOVING THE WINDOW. 
// https://billthefarmer.github.io/blog/post/handling-resizing-in-windows/ 
// https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-moving

#pragma once

#include <windowsx.h>	// GET_X_LPARAM, and such
#include <dwmapi.h>		// blur-alpha things

#include "render/render.hpp"
#include "tool/debug.hpp"
#include "loader.hpp"

// # TODO 1. RENDER_CASE FIX
//  To make window render while moving or resising I would need to handle WM_SYSCOMMAND moving resizing myself
//  and in there place my Render() calls. This is because when resizing moving we go to a different mode and we're
//  stuck (render) at different place.

#ifdef WINDOW_ALPHA
namespace WIN::ALPHA {

	// This is cheat-like implementation.
	//  Basically we're using AREO-BLUR API incorrectly to achive the effect.
	//  more about -> https://stackoverflow.com/questions/1617370/how-to-use-alpha-transparency-in-opengl#1617379
	void Enable (
		const HWND& window
	) {
		DWM_BLURBEHIND bb { 0 };
		bb.fEnable = TRUE;
		HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
		bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
		bb.hRgnBlur = hRgn;
		DwmEnableBlurBehindWindow(window, &bb);

		// Then happedns
		//  PIXELFORMATDESCRIPTOR set, glEnable(), glClearColor() with alpha value.
	}

}
#endif

#ifdef WINDOW_CUSTOM
namespace WIN::CUSTOM {

	#ifdef WINDOW_CUSTOM_WITH_SHADOW
	// This not only also allows for shadow creation but also
	//  makes a visible border around the window when using WINDOW_ALPHA flag.
	void EnableShadow (
		const HWND& window
	) {
		MARGINS margins { 1, 1, 1, 1 };
		DwmExtendFrameIntoClientArea (window, &margins);
	}
	#endif

	// When we have no border or title bar, we need to perform our
	//  own hit testing to allow resizing and moving.
	LRESULT MouseTest (
		HWND window,
		s32 x,
		s32 y
	) {

		const auto borderlessResize = true;
		const auto drag = HTCAPTION;

		const POINT border {
			GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER),
			GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER)
		};

		RECT windowArea;
		if (!GetWindowRect (window, &windowArea)) {
			return HTNOWHERE;
		}

		//RECT windowBar { 
		//	windowArea.left	  + border.x,		// left
		//	windowArea.top    + border.y,		// top
		//	windowArea.right  - border.x,		// right
		//	windowArea.top    + border.y + 100	//bottom
		//}

		enum regionMask {
			client = 0b00000000,
			left   = 0b00000001,
			right  = 0b00000010,
			top    = 0b00000100,
			bottom = 0b00001000,
			bar	   = 0b00010000,
		};

		const auto result =
			left    * (x <  (windowArea.left   + border.x)) |
			right   * (x >= (windowArea.right  - border.x)) |
			top     * (y <  (windowArea.top    + border.y)) |
			bottom  * (y >= (windowArea.bottom - border.y)) |
			bar		* (y <  (windowArea.top    + border.y + 100));

		switch (result) {
			case left + bar			:
			case left				: return borderlessResize ? HTLEFT        : drag;

			case right + bar		:
			case right				: return borderlessResize ? HTRIGHT       : drag;

			case top + bar			:
			case top				: return borderlessResize ? HTTOP         : drag;

			case bottom				: return borderlessResize ? HTBOTTOM      : drag;

			case top + left + bar	: return borderlessResize ? HTTOPLEFT     : drag;
			case top + right + bar	: return borderlessResize ? HTTOPRIGHT    : drag;
			case bottom + left		: return borderlessResize ? HTBOTTOMLEFT  : drag;
			case bottom + right		: return borderlessResize ? HTBOTTOMRIGHT : drag;

			case bar				: { /*LoadCursorA(NULL, IDC_HAND);*/ return drag; }

			case client        		: return HTCLIENT;
			default            		: return HTNOWHERE;
		}
	}

}
#endif

namespace WIN {

	void CreatePFD (
		PIXELFORMATDESCRIPTOR& pfd
	) {
		pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,                          // Version Number
			PFD_DRAW_TO_WINDOW |        // Format Must Support Window
			PFD_SUPPORT_OPENGL |        // Format Must Support OpenGL
			PFD_SUPPORT_COMPOSITION |   // Format Must Support Composition - BLUR & AREO
			PFD_DOUBLEBUFFER,           // Must Support Double Bufferin
			PFD_TYPE_RGBA,              // Request An RGBA Format
		#ifdef WINDOW_ALPHA
			32,                         // Select Our Color Depth for Framebuffer.
			0, 0, 0, 0, 0, 0,           // Color Bits Ignored
			8,                          // An Alpha Buffer was 0.
		#else
			26,                         // Select Our Color Depth for Framebuffer.
			0, 0, 0, 0, 0, 0,           // Color Bits Ignored
			0,                          // An Alpha Buffer was 0.
		#endif
			0,                          // Shift Bit Ignored
			0,                          // No Accumulation Buffer
			0, 0, 0, 0,                 // Accumulation Bits Ignored
			24,                         // Number of bits for the depthbuffer (16Bit Z-Buffer).
			8,                          // Number of bits for the stencilbuffer.
			0,                          // Number of Aux buffers in the framebuffer (NO).
			PFD_MAIN_PLANE,             // Main Drawing Layer
			0,                          // Reserved
			0, 0, 0                     // Layer Masks Ignored
		};
	}


	s64 Procedure (
		HWND window, 
		UINT message, 
		WPARAM wParam, 
		LPARAM lParam
	) {
		DEBUG { if (ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam)) return true; }

		switch (message) {

			#ifdef WINDOW_CUSTOM_WITH_SHADOW
				case WM_ACTIVATE: {
					CUSTOM::EnableShadow (window);
				} break;
			#endif

			case WM_CREATE: {

				#ifdef WINDOW_ALPHA
					ALPHA::Enable (window);
				#endif

				PIXELFORMATDESCRIPTOR pfd;
				CreatePFD(pfd);

				LOADER::CreateGlContext (window, pfd);
				LOADER::SetSwapInterval (1);

				#ifdef WINDOW_ALPHA 
					// WinAPI window alhpa is enabled but OpenAL 
					//  aLpha Render has to be set here.
					glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glEnable (GL_BLEND);
				#endif

				IMGUI::Create (window);

				DEBUG { spdlog::info ("Window succesfully Created"); }
			} break;

			case WM_SIZE: {
				RENDER::Render();
			} break;

			case WM_MOVE: {
				RENDER::Render();
			} break;

			//case WM_MOUSEMOVE: {
			//	s32 xPos = GET_X_LPARAM(lParam); 
			//	s32 yPos = GET_Y_LPARAM(lParam);
			//	DEBUG { spdlog::info ("MousePos: {0}, {1}", xPos, yPos); }
			//} break;

			#ifdef WINDOW_CUSTOM
				case WM_NCCALCSIZE: 
					return 0;
			#endif

			#ifdef WINDOW_CUSTOM
				case WM_NCHITTEST: {
					return CUSTOM::MouseTest (window, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				} break;
			#endif
	
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

		WNDCLASSEXW windowClass		{ 0 };
		windowClass.cbSize 			= sizeof(WNDCLASSEXW);
		windowClass.lpfnWndProc		= Procedure;
		windowClass.hInstance     	= instance;
		windowClass.hIcon			= nullptr; // (HICON)LoadImage(process, RESOURCE_LOCATION TEXTURE_ICON_MAIN, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
		windowClass.hbrBackground 	= nullptr; // (HBRUSH)(COLOR_BACKGROUND);
		windowClass.lpszClassName 	= windowName;
		windowClass.style 			= CS_OWNDC;

		if (!RegisterClassExW (&windowClass) ) exit (1);

		window = CreateWindowExW (
			WS_EX_LEFT,
			windowName,
			windowTitle,
			#ifdef WINDOW_CUSTOM
			WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE,
			#else
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			#endif
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
	}

	// FOR SWAPPING !!!
	void DeleteContext() {
		wglMakeCurrent (LOADER::graphicalContext, NULL);
		wglDeleteContext (LOADER::openGLRenderContext);
	}

	void Destroy (
		HINSTANCE instance,
		HWND& window,
		c16* windowName
	) { 
		wglDeleteContext (WIN::LOADER::openGLRenderContext);
		IMGUI::Destroy ();
		UnregisterClassW (windowName, instance);
	}
}