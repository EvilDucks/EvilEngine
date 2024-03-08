#pragma once

#include "tool/debug.hpp"
#include "global.hpp"
#include "gl.hpp"

#include "platform/win/loader.hpp"

namespace RENDER {

    void Update ( u16* windowSize, Color4& backgroundColor );

    void Render () {
		IMGUI::Render (*(ImVec4*)(&GLOBAL::backgroundColor));
        wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);

        Update (GLOBAL::windowSize, GLOBAL::backgroundColor);

		IMGUI::PostRender ();
        wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);

		SwapBuffers (WIN::LOADER::graphicalContext);
	}

    void Update (
        u16* windowSize,
        Color4& backgroundColor
    ) {
        glViewport (0, 0, windowSize[0], windowSize[1]); // Display Size got from Resize Command

        glClearColor (
			backgroundColor.r * backgroundColor.a, 
			backgroundColor.g * backgroundColor.a, 
			backgroundColor.b * backgroundColor.a, 
			backgroundColor.a
		);
			
        glClear (GL_COLOR_BUFFER_BIT);
    }

}