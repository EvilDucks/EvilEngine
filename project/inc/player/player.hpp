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

    struct Base {
        std::vector<InputDevice> controlScheme;
        std::string name;
        TRANSFORM::Transform* transform = nullptr;
        COLLIDER::Collider* collider = nullptr;
        PlayerMovement movement;
        glm::vec3 prevPosition;
    };

    struct Player {
        GameObjectID id = 0;

        Base local;
    };

    void PlayerMovementX (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.prevPosition = player.local.transform->local.position;
        player.local.transform->local.position = glm::vec3(player.local.transform->local.position.x + value * player.local.movement.playerSpeed, player.local.transform->local.position.y, player.local.transform->local.position.z);
        COLLIDER::UpdateColliderTransform(*player.local.collider, *player.local.transform);
        player.local.transform->flags = TRANSFORM::DIRTY;
    }

    void PlayerMovementY (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.prevPosition = player.local.transform->local.position;
        player.local.transform->local.position = glm::vec3(player.local.transform->local.position.x, player.local.transform->local.position.y, player.local.transform->local.position.z + value * player.local.movement.playerSpeed);
        player.local.transform->flags = TRANSFORM::DIRTY;
        COLLIDER::UpdateColliderTransform(*player.local.collider, *player.local.transform);
    }

    void PlayerRotation (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.transform->local.rotation.y += value * player.local.movement.rotationSpeed;
        COLLIDER::UpdateColliderTransform(*player.local.collider, *player.local.transform);
        player.local.transform->flags = TRANSFORM::DIRTY;
    }

    void MapCollision (PLAYER::Player& player, COLLIDER::Collider& collider, glm::vec3 overlap)
    {
        ZoneScopedN("Player: MapCollision");

        //TODO: more precise separation
        if (abs(overlap.x) != 0.f)
        {
            player.local.transform->local.position.x += overlap.x;
        }
        else if (abs(overlap.y) != 0.f)
        {
            player.local.transform->local.position.y += overlap.y;
        }
        else
        {
            player.local.transform->local.position.z += overlap.z;
        }
        COLLIDER::UpdateColliderTransform(*player.local.collider, *player.local.transform);
        player.local.transform->flags = TRANSFORM::DIRTY;

    }

    void HandlePlayerCollisions (PLAYER::Player& player, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount)
    {
        ZoneScopedN("Player: HandlePlayerCollisions");

        for (auto & _collision : player.local.collider->local.collisionsList)
        {
            u64 colliderIndex = OBJECT::ID_DEFAULT;
            OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, collidersCount[_collision.group], colliders[_collision.group], _collision.id);

            switch (_collision.group){
                case COLLIDER::ColliderGroup::MAP:
                    MapCollision(player, colliders[COLLIDER::ColliderGroup::MAP][colliderIndex], _collision.overlap);
                    break;
                default:
                    break;
            }
        }
        player.local.collider->local.collisionsList.clear();
    }
}