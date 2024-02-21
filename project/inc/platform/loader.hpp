// Made by Matthew Strumillo 2024-02-21
//

// More about what this is.
// https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_%28WGL%29

#pragma once

#include <windows.h>
#include <glad/glad.h>
#include <wglext.h>
#include <tchar.h>

#include "debug.hpp"

namespace WIN::LOADER {

    // =============================================================================
    //                               GLOBAL VARIABLES
    // =============================================================================

    // H - Handle, D - Device, C - Context

    HGLRC   openGLRenderContext;
    HDC     graphicalContext;
    PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT;
    PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT;

    #ifdef WIN32
    #define __MYGLEXT_GetProcAddress wglGetProcAddress
    #endif
    #ifdef GLX
    #define __MYGLEXT_GetProcAddress glXGetProcAddress
    #endif

    typedef void APIENTRY (*__MYGLEXTFP_GLGENERATEMIPMAPS)(GLenum);
    __MYGLEXTFP_GLGENERATEMIPMAPS __myglextGenerateMipmaps;
    #define glGenerateMipmaps __myglextGenerateMipmaps;


    // =============================================================================
    //                             FOWARD DECLARATIONS
    // =============================================================================
    void CreateGlContext(const HWND&);
    bool SetSwapInterval(int interval); //0 - No Interval, 1 - Sync whit VSYNC, n - n times Sync with VSYNC
    bool WGLExtensionSupported(const char *extension_name);


    void CreateGlContext (
    	const HWND& windowHandle
    ){
    	PIXELFORMATDESCRIPTOR pfd {
    		sizeof(PIXELFORMATDESCRIPTOR),
    		1,                          // Version Number
    		PFD_DRAW_TO_WINDOW |        // Format Must Support Window
            PFD_SUPPORT_OPENGL |        // Format Must Support OpenGL
            PFD_SUPPORT_COMPOSITION |   // Format Must Support Composition - BLUR & AREO
            PFD_DOUBLEBUFFER,           // Must Support Double Bufferin
    		PFD_TYPE_RGBA,              // Request An RGBA Format
    		32,                         // Select Our Color Depth for Framebuffer.
    		0, 0, 0, 0, 0, 0,           // Color Bits Ignored
    		8,                          // An Alpha Buffer was 0.
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

    	graphicalContext = GetDC(windowHandle);  

        int pixelFormal = ChoosePixelFormat (graphicalContext, &pfd); 
        SetPixelFormat (graphicalContext, pixelFormal, &pfd);
        openGLRenderContext = wglCreateContext (graphicalContext);
        wglMakeCurrent (graphicalContext, openGLRenderContext);

        gladLoadGL();

        DEBUG { 
            // If this throws an ERROR then the "gladLoadGL" failed! and returned false.
            spdlog::info ( "OpenGL Version: {0}", (char*)glGetString(GL_VERSION) ); 
        }
    }

    bool SetSwapInterval(int interval) {
    
        if (WGLExtensionSupported("WGL_EXT_swap_control")) {
            // Extension is supported, init pointers.
            wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress ("wglSwapIntervalEXT");

            // this is another function from WGL_EXT_swap_control extension
            wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC) wglGetProcAddress ("wglGetSwapIntervalEXT");

            wglSwapIntervalEXT(interval);
            return true;
        }

        return false;
    }

    //Got from https://stackoverflow.com/questions/589064/how-to-enable-vertical-sync-in-opengl/589232
    bool WGLExtensionSupported(const char *extension_name){
        // this is pointer to function which returns pointer to string with list of all wgl extensions
        PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

        // determine pointer to wglGetExtensionsStringEXT function
        _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");

        if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL) {
            // string was not found
            return false;
        }
    
    	__myglextGenerateMipmaps = 
        (__MYGLEXTFP_GLGENERATEMIPMAPS) __MYGLEXT_GetProcAddress("glGenerateMipmap");

        // extension is supported
        return true;
    }

}