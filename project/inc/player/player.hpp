//
// Created by Bartek on 08.04.2024.
//

#ifndef EVILENGINE_PLAYER_HPP
#define EVILENGINE_PLAYER_HPP

#endif //EVILENGINE_PLAYER_HPP

#include <iostream>
#include "tool/debug.hpp"
#include "render/mesh.hpp"
#include "hid/inputManager.hpp"


namespace PLAYER {

    struct PlayerMovement {
        float playerSpeed = 0.05f;
        float rotationSpeed = 0.5f;
    };

    struct SelectionPosition {
        unsigned int x = 0;
        unsigned int y = 0;
    };

    struct Base {
        std::vector<InputDevice> controlScheme;
        std::string name;
        TRANSFORM::LTransform* transform = nullptr;
        COLLIDER::Collider* collider = nullptr;
        PlayerMovement movement;
        glm::vec3 prevPosition;
        SelectionPosition selection;
    };

    struct Player {
        GameObjectID id = 0;

        Base local;
    };

    void PlayerMovementX (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.prevPosition = player.local.transform->base.position;
        player.local.transform->base.position = glm::vec3(player.local.transform->base.position.x + value * player.local.movement.playerSpeed, player.local.transform->base.position.y, player.local.transform->base.position.z);
        COLLIDER::UpdateColliderTransform(*player.local.collider, *player.local.transform);
        player.local.transform->flags = TRANSFORM::DIRTY;
    }

    void PlayerMovementY (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.prevPosition = player.local.transform->base.position;
        player.local.transform->base.position = glm::vec3(player.local.transform->base.position.x, player.local.transform->base.position.y, player.local.transform->base.position.z + value * player.local.movement.playerSpeed);
        player.local.transform->flags = TRANSFORM::DIRTY;
        COLLIDER::UpdateColliderTransform(*player.local.collider, *player.local.transform);
    }

    void PlayerRotation (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.transform->base.rotation.y += value * player.local.movement.rotationSpeed;
        COLLIDER::UpdateColliderTransform(*player.local.collider, *player.local.transform);
        player.local.transform->flags = TRANSFORM::DIRTY;
    }

    void MapCollision (PLAYER::Player& player, COLLIDER::Collider& collider, glm::vec3 overlap)
    {
        PROFILER { ZoneScopedN("Player: MapCollision"); }

        //TODO: more precise separation
        if (abs(overlap.x) != 0.f)
        {
            player.local.transform->base.position.x += overlap.x;
        }
        else if (abs(overlap.y) != 0.f)
        {
            player.local.transform->base.position.y += overlap.y;
        }
        else
        {
            player.local.transform->base.position.z += overlap.z;
        }
        COLLIDER::UpdateColliderTransform(*player.local.collider, *player.local.transform);
        player.local.transform->flags = TRANSFORM::DIRTY;

    }

    void HandlePlayerCollisions (PLAYER::Player& player, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount)
    {
        PROFILER { ZoneScopedN("Player: HandlePlayerCollisions"); }

        for (int i = 0; i < player.local.collider->local.collisionsList.size(); i++)
        {
            COLLIDER::Collision _collision = player.local.collider->local.collisionsList[i];
            u64 colliderIndex = OBJECT::ID_DEFAULT;
            OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, collidersCount[_collision.group], colliders[_collision.group], _collision.id);

            switch (_collision.group){
                case COLLIDER::ColliderGroup::MAP:
                    MapCollision(player, colliders[COLLIDER::ColliderGroup::MAP][colliderIndex], _collision.overlap);
                    break;
                default:
                    break;
            }

            colliders[_collision.group][colliderIndex].local.collisionsList.erase(colliders[_collision.group][colliderIndex].local.collisionsList.begin() + COLLIDER::FindCollisionIndexById(colliders[_collision.group][colliderIndex], player.id));
            player.local.collider->local.collisionsList.erase(player.local.collider->local.collisionsList.begin() + i);
        }

    }
}