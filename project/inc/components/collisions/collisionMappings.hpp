//
// Created by Bartek on 23.05.2024.
//

#ifndef EVILENGINE_COLLISIONMAPPINGS_HPP
#define EVILENGINE_COLLISIONMAPPINGS_HPP

#include "global.hpp"
#include <chrono>
#include <thread>

using std::operator""s;

namespace COLLISION_MAP {
    void RegisterCallbacks(COLLISION::MANAGER::CM& manager)
    {
        COLLISION::MANAGER::RegisterCollisionCallback(manager, "Goal", COLLISION::MANAGER::CollisionCallback{
                .Ref = "GameplayCollision",
                .Func = [](COLLIDER::Collider collider1, COLLIDER::Collider collider2, glm::vec3 overlap) {

                    DEBUG {spdlog::info("test trigger, overlap [x: {0}; y: {1}; z: {2}", overlap.x, overlap.y, overlap.z);}

                    GLOBAL::world.lTransforms[GLOBAL::world.players[0].local.transformIndex].base.position = glm::vec3(0.0f, 0.0f, 2.0f);
                    GLOBAL::world.lTransforms[GLOBAL::world.players[1].local.transformIndex].base.position = glm::vec3(2.0f, 0.0f, 2.0f);

                    return true;
                }
        });

        COLLISION::MANAGER::RegisterCollisionCallback(manager, "SpringTrap", COLLISION::MANAGER::CollisionCallback{
                .Ref = "GameplayCollision",
                .Func = [](COLLIDER::Collider collider1, COLLIDER::Collider collider2, glm::vec3 overlap) {

                    u64 playerIndex = OBJECT::ID_DEFAULT;
                    OBJECT::GetComponentFast<PLAYER::Player>(playerIndex, GLOBAL::world.playersCount, GLOBAL::world.players, collider2.id);

                    SPRING_TRAP::Activation(collider1, GLOBAL::world.players[playerIndex], GLOBAL::world.lTransforms, GLOBAL::world.rigidbodies);

                    u64 colliderIndex = OBJECT::ID_DEFAULT;
                    OBJECT::GetComponentFast<COLLIDER::Collider>(colliderIndex, GLOBAL::world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], GLOBAL::world.colliders[COLLIDER::ColliderGroup::TRIGGER], collider1.id);
                    GLOBAL::world.colliders[COLLIDER::ColliderGroup::TRIGGER][colliderIndex].local.isEnabled = false;

                    return true;
                }
        });

        COLLISION::MANAGER::RegisterCollisionCallback(manager, "PowerUp", COLLISION::MANAGER::CollisionCallback{
                .Ref = "GameplayCollision",
                .Func = [](COLLIDER::Collider collider1, COLLIDER::Collider collider2, glm::vec3 overlap) {

                    u64 playerIndex = OBJECT::ID_DEFAULT;
                    OBJECT::GetComponentFast<PLAYER::Player>(playerIndex, GLOBAL::world.playersCount, GLOBAL::world.players, collider2.id);

                    if (GLOBAL::world.players[playerIndex].local.powerUp == POWER_UP::PowerUpType::NONE)
                    {
                        GLOBAL::world.players[playerIndex].local.powerUp = POWER_UP::PickUpPowerUp();
                        u64 colliderIndex = OBJECT::ID_DEFAULT;
                        OBJECT::GetComponentFast<COLLIDER::Collider>(colliderIndex, GLOBAL::world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], GLOBAL::world.colliders[COLLIDER::ColliderGroup::TRIGGER], collider1.id);
                        GLOBAL::world.colliders[COLLIDER::ColliderGroup::TRIGGER][colliderIndex].local.isEnabled = false;
                        u64 transformIndex = OBJECT::ID_DEFAULT;
                        OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, GLOBAL::world.transformsCount, GLOBAL::world.lTransforms, collider1.id);
                        GLOBAL::world.lTransforms[transformIndex].base.position.y = -100.f;
                        GLOBAL::world.lTransforms[transformIndex].flags = TRANSFORM::DIRTY;
                    }

                    return true;
                }
        });

        COLLISION::MANAGER::RegisterCollisionCallback(manager, "CheckPoint", COLLISION::MANAGER::CollisionCallback{
                .Ref = "GameplayCollision",
                .Func = [](COLLIDER::Collider collider1, COLLIDER::Collider collider2, glm::vec3 overlap) {

                    u64 playerIndex = OBJECT::ID_DEFAULT;
                    OBJECT::GetComponentFast<PLAYER::Player>(playerIndex, GLOBAL::world.playersCount, GLOBAL::world.players, collider2.id);

                    u64 checkpointIndex = OBJECT::ID_DEFAULT;
                    OBJECT::GetComponentFast<CHECKPOINT::Checkpoint>(checkpointIndex, GLOBAL::world.checkpointsCount, GLOBAL::world.checkpoints, collider1.id);

                    GLOBAL::world.players[playerIndex].local.checkPointInRange = checkpointIndex;

                    DEBUG { spdlog::info("checkpoint collision index: {0}", checkpointIndex);}

                    return true;
                }
        });
    }
}


#endif //EVILENGINE_COLLISIONMAPPINGS_HPP
