//
// Created by Bartek on 23.05.2024.
//

#ifndef EVILENGINE_COLLISIONMAPPINGS_HPP
#define EVILENGINE_COLLISIONMAPPINGS_HPP

#include "global.hpp"

namespace COLLISION_MAP {
    void RegisterCallbacks(COLLISION::MANAGER::CM& manager)
    {
        COLLISION::MANAGER::RegisterCollisionCallback(manager, "testTrigger", COLLISION::MANAGER::CollisionCallback{
                .Ref = "GameplayCollision",
                .Func = [](COLLIDER::Collider collider1, COLLIDER::Collider collider2, glm::vec3 overlap) {

                    DEBUG {spdlog::info("test trigger, overlap [x: {0}; y: {1}; z: {2}", overlap.x, overlap.y, overlap.z);}

                    return true;
                }
        });
    }
}


#endif //EVILENGINE_COLLISIONMAPPINGS_HPP
