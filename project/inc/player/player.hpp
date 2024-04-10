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
        PlayerMovement movement;
    };

    struct Player {
        GameObjectID id = 0;

        Base local;
    };

    void PlayerMovementX (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.transform->local.position = glm::vec3(player.local.transform->local.position.x + value * player.local.movement.playerSpeed, player.local.transform->local.position.y, player.local.transform->local.position.z);
        player.local.transform->flags = TRANSFORM::DIRTY;
    }

    void PlayerMovementY (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.transform->local.position = glm::vec3(player.local.transform->local.position.x, player.local.transform->local.position.y, player.local.transform->local.position.z + value * player.local.movement.playerSpeed);
        player.local.transform->flags = TRANSFORM::DIRTY;
    }
}