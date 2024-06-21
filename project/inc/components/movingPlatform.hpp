//
// Created by Bartek on 20.06.2024.
//

#ifndef EVILENGINE_MOVINGPLATFORM_HPP
#define EVILENGINE_MOVINGPLATFORM_HPP

#include <iostream>
#include "tool/debug.hpp"
#include "render/gl.hpp"
#include "transform.hpp"

namespace MOVING_PLATFORM {

    struct Base {
        u16 segmentIndex;
        u16 transformIndex;
        u16 mapColliderIndex;
        u16 triggerColliderIndex;
        glm::vec3 position = glm::vec3(0.f);
        glm::vec3 travelDistance = glm::vec3(0.f);
        float travelTime = 1.f;
        int direction = 1;
        glm::vec3 step = glm::vec3(0.f);
    };

    struct MovingPlatform {
        GameObjectID id = 0;
        Base base;
    };

    void MovePlatform (MOVING_PLATFORM::MovingPlatform& movingPlatform, TRANSFORM::LTransform& transform, float deltaTime)
    {
        auto& position = movingPlatform.base.position;
        auto& travelDistance = movingPlatform.base.travelDistance;

        glm::vec3 step = movingPlatform.base.direction * movingPlatform.base.travelTime * travelDistance * deltaTime;

        // Check if platform reached one of its destinations
        if (movingPlatform.base.direction == 1 && (position.x + step.x> travelDistance.x || position.y + step.y > travelDistance.y || position.z + step.z > travelDistance.z))
        {
            step = travelDistance - position;
            movingPlatform.base.direction = -1;
        }
        else if (movingPlatform.base.direction == -1 && (position.x + step.x < 0  || position.y + step.y < 0 || position.z + step.z < 0))
        {
            step = -position;
            movingPlatform.base.direction = 1;
        }

        movingPlatform.base.step = step;
        position += step;
        transform.base.position += step;
        transform.flags = TRANSFORM::DIRTY;
    }

//    void MovePlatforms (MOVING_PLATFORM::MovingPlatform* movingPlatforms, u16 movingPlatformsCount, TRANSFORM::LTransform* transforms)
//    {
//        for (int i = 0; i < movingPlatformsCount; i++)
//        {
//            MovePlatform(movingPlatforms[i]);
//        }
//    }

}

#endif //EVILENGINE_MOVINGPLATFORM_HPP
