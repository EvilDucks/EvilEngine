//
// Created by Bartek on 08.04.2024.
//

#ifndef EVILENGINE_PLAYER_HPP
#define EVILENGINE_PLAYER_HPP



#include <iostream>
#include "tool/debug.hpp"
#include "render/mesh.hpp"
#include "hid/inputManager.hpp"
#include "playerMovement.hpp"
#include "../components/rigidbody.hpp"

namespace PLAYER {

    struct JumpData {
        u8 maxJumps = 2;
        u8 jumpsCount = 0;
        float jumpRange = 3.f;
        float jumpHeight = 0.5f;
    };

    struct MovementValue {
        float forward;
        float right;
    };

    struct PlayerMovement {
        MovementValue movementValue;
        float yaw;
        glm::vec3 velocity = glm::vec3(0.f);
        float playerSpeed = 5.0f;
        float rotationSpeed = 0.5f;
        float gravitation = 0.25f;
        JumpData jumpData;
        bool movementLock = false;
    };

    struct SelectionPosition {
        unsigned int x = 0;
        unsigned int y = 0;
    };

    struct Base {
        std::vector<InputDevice> controlScheme;
        std::string name;
        u64 transformIndex = 0;
        u64 colliderIndex = 0;
        u64 rigidbodyIndex = 0;
        COLLIDER::ColliderGroup colliderGroup = COLLIDER::ColliderGroup::PLAYER;
        PlayerMovement movement;
        glm::vec3 prevPosition;
        SelectionPosition selection;
    };

    struct Player {
        GameObjectID id = 0;

        Base local;
    };

    void PlatformLanding (PLAYER::Player& player, RIGIDBODY::Rigidbody* rigidbodies)
    {
        rigidbodies[player.local.rigidbodyIndex].base.velocity.y = 0;
        player.local.movement.jumpData.jumpsCount = 0;
    }

    void PlayerRotation (PLAYER::Player& player, float value, InputContext context, TRANSFORM::LTransform* transforms, TRANSFORM::GTransform* globalTransforms, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders)
    {
        transforms[player.local.transformIndex].base.rotation.y += value * player.local.movement.rotationSpeed;
        COLLIDER::UpdateColliderTransform(colliders[player.local.colliderGroup][player.local.colliderIndex], globalTransforms[player.local.transformIndex]);
        transforms[player.local.transformIndex].flags = TRANSFORM::DIRTY;
    }

    void MapCollision (PLAYER::Player& player, glm::vec3 overlap, TRANSFORM::LTransform* transforms, RIGIDBODY::Rigidbody* rigidbodies)
    {
        PROFILER { ZoneScopedN("Player: MapCollision"); }

        if (abs(overlap.x) != 0.f)
        {
            transforms[player.local.transformIndex].base.position.x += overlap.x;
            RIGIDBODY::ResetForcesX(rigidbodies[player.local.rigidbodyIndex], overlap.x);
        }
        else if (abs(overlap.y) != 0.f)
        {
            if (overlap.y > 0.f)
            {
                PlatformLanding(player, rigidbodies);
            }
            else
            {
                if (rigidbodies[player.local.rigidbodyIndex].base.velocity.y > 0)
                {
                    rigidbodies[player.local.rigidbodyIndex].base.velocity.y = 0;
                }
            }
            transforms[player.local.transformIndex].base.position.y += overlap.y;
            RIGIDBODY::ResetForcesY(rigidbodies[player.local.rigidbodyIndex], overlap.y);
        }
        else
        {
            transforms[player.local.transformIndex].base.position.z += overlap.z;
            RIGIDBODY::ResetForcesZ(rigidbodies[player.local.rigidbodyIndex], overlap.z);
        }
        transforms[player.local.transformIndex].flags = TRANSFORM::DIRTY;

    }

    void PlayerCollision (PLAYER::Player& player, COLLIDER::Collider& collider, glm::vec3 overlap, TRANSFORM::LTransform* transforms, u64 transformsCount, RIGIDBODY::Rigidbody* rigidbodies)
    {
        PROFILER { ZoneScopedN("Player: PlayerCollision"); }

        u64 transformIndex = OBJECT::ID_DEFAULT;
        OBJECT::GetComponentSlow<TRANSFORM::LTransform>(transformIndex, transformsCount, transforms, collider.id);

        if (abs(overlap.x) != 0.f)
        {
            transforms[player.local.transformIndex].base.position.x += overlap.x * 0.5f;
            transforms[transformIndex].base.position.x -= overlap.x * 0.5f;
            RIGIDBODY::ResetForcesX(rigidbodies[player.local.rigidbodyIndex], overlap.x);
        }
        else if (abs(overlap.y) != 0.f)
        {
            if (overlap.y > 0.f)
            {
                PlatformLanding(player, rigidbodies);
            }
            transforms[player.local.transformIndex].base.position.y += overlap.y * 0.5f;
            transforms[transformIndex].base.position.y -= overlap.y * 0.5f;
            RIGIDBODY::ResetForcesY(rigidbodies[player.local.rigidbodyIndex], overlap.y);
        }
        else
        {
            transforms[player.local.transformIndex].base.position.z += overlap.z * 0.5f;
            transforms[transformIndex].base.position.z -= overlap.z * 0.5f;
            RIGIDBODY::ResetForcesZ(rigidbodies[player.local.rigidbodyIndex], overlap.z);
        }

        transforms[player.local.transformIndex].flags = TRANSFORM::DIRTY;
        transforms[transformIndex].flags = TRANSFORM::DIRTY;
    }

    void HandlePlayerCollisions (PLAYER::Player& player, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount, TRANSFORM::LTransform* transforms, TRANSFORM::GTransform* globalTransforms, u64 transformsCount, RIGIDBODY::Rigidbody* rigidbodies)
    {
        PROFILER { ZoneScopedN("Player: HandlePlayerCollisions"); }

        for (int i = colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList.size() - 1; i >= 0; i--)
        {
            COLLIDER::Collision _collision = colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList[i];
            u64 colliderIndex = OBJECT::ID_DEFAULT;
            OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, collidersCount[_collision.group], colliders[_collision.group], _collision.id);

            switch (_collision.group){
                case COLLIDER::ColliderGroup::MAP:
                    MapCollision(player, _collision.overlap, transforms, rigidbodies);
                    break;
                case COLLIDER::ColliderGroup::PLAYER:
                    PlayerCollision(player, colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex], _collision.overlap, transforms, transformsCount, rigidbodies);
                    break;
                default:
                    break;
            }
            auto v = colliders[_collision.group][colliderIndex].local.collisionsList;
            colliders[_collision.group][colliderIndex].local.collisionsList.erase(colliders[_collision.group][colliderIndex].local.collisionsList.begin() + COLLIDER::FindCollisionIndexById(colliders[_collision.group][colliderIndex], player.id));
            colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList.erase(colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList.begin() + i);
        }
    }
}

#endif //EVILENGINE_PLAYER_HPP