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

    void Collision (PLAYER::Player& player)
    {
        if (player.local.collider->local.collision)
        {
            player.local.transform->local.position = player.local.prevPosition;
            COLLIDER::UpdateColliderTransform(*player.local.collider, *player.local.transform);

            player.local.transform->flags = TRANSFORM::DIRTY;
            player.local.collider->local.collision = false;
        }

    }
}