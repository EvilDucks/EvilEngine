#pragma once

#include <iostream>
#include "glm/mat4x4.hpp"
#include "object.hpp"

namespace CHECKPOINT
{
    struct Checkpoint {
        GameObjectID id = 0;
        glm::vec3 position;
    };
}