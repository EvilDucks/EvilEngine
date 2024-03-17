#pragma once

#include "tool/debug.hpp"
#include "global.hpp"
#include "gl.hpp"

#ifdef PLATFORM_WINDOWS_TOKEN
#include "platform/win/loader.hpp"
#endif

#include "scenes.hpp"

namespace RENDER {

    void Update ( u16* windowSize, Color4& backgroundColor, SCENES::SceneTree& sceneTree );

    void Render () {
		IMGUI::Render (*(ImVec4*)(&GLOBAL::backgroundColor));
        wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);

        Update (GLOBAL::windowSize, GLOBAL::backgroundColor, GLOBAL::sceneTree);

		IMGUI::PostRender ();
        wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);

		SwapBuffers (WIN::LOADER::graphicalContext);
	}

    void Update (
        u16* windowSize,
        Color4& backgroundColor,
        SCENES::SceneTree& sceneTree
    ) {
        glViewport (0, 0, windowSize[0], windowSize[1]); // Display Size got from Resize Command

        glClearColor (
			backgroundColor.r * backgroundColor.a, 
			backgroundColor.g * backgroundColor.a, 
			backgroundColor.b * backgroundColor.a, 
			backgroundColor.a
		);

        glClear (GL_COLOR_BUFFER_BIT);

        glUseProgram(sceneTree.programId);
        glBindVertexArray(sceneTree.verticesId); // BOUND VAO

        // Without EBO
        // const u8 OFFSET = 0;
        //glDrawArrays(GL_TRIANGLES, OFFSET, sceneTree.verticiesCount);

        // With EBO
        const void* USING_VBO = nullptr;
        glDrawElements(GL_TRIANGLES, sceneTree.verticiesCount, GL_UNSIGNED_INT, USING_VBO);

        glBindVertexArray(0); // UNBOUND VAO
			
        
    }

}