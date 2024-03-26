#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"

namespace TRANSFORM {

    //struct Position {
    //    r32 x, y, z;
    //};
    //
    //struct Rotation {
    //    r32 x, y, z;
    //};
    //
    //struct Scale {
    //    r32 x, y, z;
    //};
    //
    //struct Base {
    //    Position position;
    //    Rotation rotation;
    //    Scale scale;
    //};
    //
    //struct Transform {
    //    GameObjectID id = 0;
    //    Base base { 0 };
    //};

    struct Transform {
        GameObjectID id = 0;
        glm::mat4 base = glm::mat4(1.0f);
    };

}