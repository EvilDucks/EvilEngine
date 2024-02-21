#pragma once

#include "types.hpp"
#include "gl.hpp"

namespace RENDER {

    void Update (
        u16* windowSize,
        ImVec4& backgroundColor
    ) {
        //spdlog::info ("A"); 

        glViewport(0, 0, windowSize[0], windowSize[1]); // Display Size got from Resize Command

        //spdlog::info ("B"); 

        glClearColor(
			backgroundColor.x * backgroundColor.w, 
			backgroundColor.y * backgroundColor.w, 
			backgroundColor.z * backgroundColor.w, 
			backgroundColor.w
		);

        //spdlog::info ("C"); 
			
        glClear(GL_COLOR_BUFFER_BIT);

        //spdlog::info ("D"); 
    }

}