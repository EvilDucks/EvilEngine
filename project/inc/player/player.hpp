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

namespace PLAYER {

    struct JumpData {
        u8 maxJumps = 2;
        u8 jumpsCount = 0;
        float jumpRange = 2.f;
        float jumpHeight = 5.f;
    };

    struct PlayerMovement {
        glm::vec3 velocity = glm::vec3(0.f);
        float playerSpeed = 0.05f;
        float rotationSpeed = 0.5f;
        float gravitation = 0.25f;
        JumpData jumpData;
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
        COLLIDER::ColliderGroup colliderGroup = COLLIDER::ColliderGroup::PLAYER;
        PlayerMovement movement;
        glm::vec3 prevPosition;
        SelectionPosition selection;
    };

    struct Player {
        GameObjectID id = 0;

        Base local;
    };

    void PlatformLanding (PLAYER::Player& player)
    {
        player.local.movement.velocity.y = 0.f;
        player.local.movement.jumpData.jumpsCount = 0;
    }

    void PlayerRotation (PLAYER::Player& player, float value, InputContext context, TRANSFORM::LTransform* transforms, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders)
    {
        transforms[player.local.transformIndex].local.rotation.y += value * player.local.movement.rotationSpeed;
        COLLIDER::UpdateColliderTransform(colliders[player.local.colliderGroup][player.local.colliderIndex], transforms[player.local.transformIndex]);
        transforms[player.local.transformIndex].flags = TRANSFORM::DIRTY;
    }

    void MapCollision (PLAYER::Player& player, COLLIDER::Collider& collider, glm::vec3 overlap, TRANSFORM::LTransform* transforms, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders)
    {
        PROFILER { ZoneScopedN("Player: MapCollision"); }

        //TODO: more precise separation
        if (abs(overlap.x) != 0.f)
        {
            transforms[player.local.transformIndex].local.position.x += overlap.x;
        }
        else if (abs(overlap.y) != 0.f)
        {
            if (overlap.y > 0.f)
            {
                PlatformLanding(player);
            }
            transforms[player.local.transformIndex].local.position.y += overlap.y;
        }
        else
        {
            transforms[player.local.transformIndex].local.position.z += overlap.z;
        }
        COLLIDER::UpdateColliderTransform(colliders[player.local.colliderGroup][player.local.colliderIndex], transforms[player.local.transformIndex]);
        transforms[player.local.transformIndex].flags = TRANSFORM::DIRTY;

    }

    void HandlePlayerCollisions (PLAYER::Player& player, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount, TRANSFORM::LTransform* transforms)
    {
        PROFILER { ZoneScopedN("Player: HandlePlayerCollisions"); }

        for (int i = 0; i < colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList.size(); i++)
        {
            COLLIDER::Collision _collision = colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList[i];
            u64 colliderIndex = OBJECT::ID_DEFAULT;
            OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, collidersCount[_collision.group], colliders[_collision.group], _collision.id);

            switch (_collision.group){
                case COLLIDER::ColliderGroup::MAP:
                    MapCollision(player, colliders[COLLIDER::ColliderGroup::MAP][colliderIndex], _collision.overlap, transforms, colliders);
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