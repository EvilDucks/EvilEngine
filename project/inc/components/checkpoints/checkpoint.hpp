#pragma once

#include <iostream>
#include "glm/mat4x4.hpp"

namespace CHECKPOINT
{
    struct Transform{
        glm::vec3 position;
    };

    struct Checkpoint {
        Transform transform;

    };
}