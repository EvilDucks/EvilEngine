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
        float jumpHeight = 1.5f;
    };

    struct ChargeData {
        float distance = 5.f;
        float duration = 0.5f;
        float movementLockDuration = 0.5f;
        float knockbackDistance = 7.f;
        float knockbackDuration = 0.5f;
        glm::vec3 chargeDirection = glm::vec3(0.f);
        float chargeTimer = 0.f;
    };

    struct MovementValue {
        float forward;
        float right;
    };

    struct PlayerMovement {
        glm::vec3 direction = glm::vec3(1.f, 0.f, 0.f);
        MovementValue movementValue;
        float yaw;
        glm::vec3 velocity = glm::vec3(0.f);
        float playerSpeed = 5.0f;
        float rotationSpeed = 0.5f;
        float gravitation = 0.25f;
        JumpData jumpData;
        bool movementLock = false;
        float movementLockTimer = 0.f;
        ChargeData chargeData;
    };

    struct SelectionPosition {
        unsigned int x = 0;
        unsigned int y = 0;
    };

    struct Base {
        int playerIndex = -1;
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

    void MovementLock (PLAYER::Player& player, float timer)
    {
        player.local.movement.movementLock = true;
        player.local.movement.movementLockTimer = timer;
    }

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

    void PlayerPush (PLAYER::Player& player, PLAYER::Player& otherPlayer, RIGIDBODY::Rigidbody* rigidbodies)
    {
        if (player.local.movement.chargeData.chargeTimer > 0.f && otherPlayer.local.movement.chargeData.chargeTimer > 0.f)
        {
            RIGIDBODY::ResetForce(rigidbodies[otherPlayer.local.rigidbodyIndex]);
            RIGIDBODY::ResetForce(rigidbodies[player.local.rigidbodyIndex]);
            MovementLock(otherPlayer, player.local.movement.chargeData.knockbackDuration);
            MovementLock(player, player.local.movement.chargeData.knockbackDuration);
            RIGIDBODY::AddForce(rigidbodies[otherPlayer.local.rigidbodyIndex], player.local.movement.chargeData.chargeDirection, player.local.movement.chargeData.knockbackDistance, player.local.movement.chargeData.knockbackDuration, -1);
            RIGIDBODY::AddForce(rigidbodies[player.local.rigidbodyIndex], otherPlayer.local.movement.chargeData.chargeDirection, player.local.movement.chargeData.knockbackDistance, player.local.movement.chargeData.knockbackDuration, -1);
        }
        else if (player.local.movement.chargeData.chargeTimer > 0.f)
        {
            RIGIDBODY::ResetForce(rigidbodies[otherPlayer.local.rigidbodyIndex]);
            RIGIDBODY::ResetForce(rigidbodies[player.local.rigidbodyIndex]);
            MovementLock(otherPlayer, player.local.movement.chargeData.knockbackDuration);
            RIGIDBODY::AddForce(rigidbodies[otherPlayer.local.rigidbodyIndex], player.local.movement.chargeData.chargeDirection, player.local.movement.chargeData.knockbackDistance, player.local.movement.chargeData.knockbackDuration, -1);
        }
        else if (otherPlayer.local.movement.chargeData.chargeTimer > 0.f)
        {
            RIGIDBODY::ResetForce(rigidbodies[otherPlayer.local.rigidbodyIndex]);
            RIGIDBODY::ResetForce(rigidbodies[player.local.rigidbodyIndex]);
            MovementLock(player, player.local.movement.chargeData.knockbackDuration);
            RIGIDBODY::AddForce(rigidbodies[player.local.rigidbodyIndex], otherPlayer.local.movement.chargeData.chargeDirection, player.local.movement.chargeData.knockbackDistance, player.local.movement.chargeData.knockbackDuration, -1);
        }
    }

    bool MapCollision (PLAYER::Player& player, glm::vec3 overlap, TRANSFORM::LTransform* transforms, RIGIDBODY::Rigidbody* rigidbodies)
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
        return true;
    }

    bool PlayerCollision (PLAYER::Player& player, COLLIDER::Collider& collider, glm::vec3 overlap, TRANSFORM::LTransform* transforms, u64 transformsCount, RIGIDBODY::Rigidbody* rigidbodies, PLAYER::Player& otherPlayer)
    {
        PROFILER { ZoneScopedN("Player: PlayerCollision"); }

        u64 transformIndex = OBJECT::ID_DEFAULT;
        OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, transformsCount, transforms, collider.id);

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

        PlayerPush(player, otherPlayer, rigidbodies);

        transforms[player.local.transformIndex].flags = TRANSFORM::DIRTY;
        transforms[transformIndex].flags = TRANSFORM::DIRTY;
        return true;
    }

    void HandlePlayerCollisions (PLAYER::Player& player, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount, TRANSFORM::LTransform* transforms, TRANSFORM::GTransform* globalTransforms, u64 transformsCount, RIGIDBODY::Rigidbody* rigidbodies, PLAYER::Player& otherPlayer)
    {
        PROFILER { ZoneScopedN("Player: HandlePlayerCollisions"); }

        for (int i = colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList.size() - 1; i >= 0; i--)
        {
            COLLIDER::Collision _collision = colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList[i];
            bool deleteOtherCollision = true;
            switch (_collision.group){
                case COLLIDER::ColliderGroup::MAP:
                    deleteOtherCollision = MapCollision(player, _collision.overlap, transforms, rigidbodies);
                    break;
                case COLLIDER::ColliderGroup::PLAYER:
                    deleteOtherCollision = PlayerCollision(player, colliders[COLLIDER::ColliderGroup::PLAYER][_collision.index], _collision.overlap, transforms, transformsCount, rigidbodies, otherPlayer);
                    break;
                case COLLIDER::ColliderGroup::TRIGGER:
                    deleteOtherCollision = false;
                    break;
                default:
                    break;
            }
            auto v = colliders[_collision.group][_collision.index].local.collisionsList;
            if (deleteOtherCollision)
            {
                colliders[_collision.group][_collision.index].local.collisionsList.erase(colliders[_collision.group][_collision.index].local.collisionsList.begin() + COLLIDER::FindCollisionIndex(colliders[_collision.group][_collision.index], player.local.playerIndex, player.local.colliderGroup));
            }
            colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList.erase(colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList.begin() + i);
        }
    }
}

#endif //EVILENGINE_PLAYER_HPP