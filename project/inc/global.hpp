#pragma once

#include "types.hpp"

namespace SCENES {

    struct SceneTree {
        GLuint verticesId;
        GLuint programId;
    };

}

namespace GLOBAL {

    Color4 backgroundColor = Color4 ( 114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f );
	u16 windowSize[2] { 1200, 640 };


    #define D_SHADERS "res/shaders/"

    // Shader Vertex FilePath
    const char* svfTriangle = "res/shaders/Triangle.vert";
    const char* sffTriangle = "res/shaders/Triangle.frag";

    // SET DURING INITIALIZATION
    SCENES::SceneTree sceneTree { 0 };


}