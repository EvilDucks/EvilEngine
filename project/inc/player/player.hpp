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
        float playerSpeed = 0.25f;
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

    void MapCollision (PLAYER::Player& player, COLLIDER::Collider& collider)
    {
        //TODO: more precise separation
        player.local.transform->local.position = player.local.prevPosition;
        COLLIDER::UpdateColliderTransform(*player.local.collider, *player.local.transform);
        player.local.transform->flags = TRANSFORM::DIRTY;

    }

    void HandlePlayerCollisions (PLAYER::Player& player, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount)
    {
        for (auto & _collision : player.local.collider->local.collisionsList)
        {
            u64 colliderIndex = OBJECT::ID_DEFAULT;
            OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, collidersCount[_collision.group], colliders[_collision.group], _collision.id);

            switch (_collision.group){
                case COLLIDER::ColliderGroup::MAP:
                    MapCollision(player, colliders[COLLIDER::ColliderGroup::MAP][colliderIndex]);
                    break;
                default:
                    break;
            }
        }
        player.local.collider->local.collisionsList.clear();
    }
}