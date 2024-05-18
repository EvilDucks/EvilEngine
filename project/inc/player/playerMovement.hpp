//
// Created by Bartek on 18.05.2024.
//

#ifndef EVILENGINE_PLAYERMOVEMENT_HPP
#define EVILENGINE_PLAYERMOVEMENT_HPP

#include "player.hpp"

namespace PLAYER::MOVEMENT {

    void Move(PLAYER::Player player)
    {
        // Apply gravitation
        //player.local.movement.velocity.y -= player.local.movement.gravitation;

        player.local.transform->local.position.x = player.local.transform->local.position.x + player.local.movement.velocity.x * player.local.movement.playerSpeed;
        player.local.transform->local.position.y = player.local.transform->local.position.y + player.local.movement.velocity.y * player.local.movement.playerSpeed;
        player.local.transform->local.position.z = player.local.transform->local.position.z + player.local.movement.velocity.z * player.local.movement.playerSpeed;

        player.local.transform->flags = TRANSFORM::DIRTY;

    }

    void Horizontal (PLAYER::Player& player, float value, InputContext context)
    {

        player.local.movement.velocity.x = value;
        COLLIDER::UpdateColliderTransform(*player.local.collider, *player.local.transform);
        player.local.transform->flags = TRANSFORM::DIRTY;
    }

    void Vertical (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.movement.velocity.z = value;
        COLLIDER::UpdateColliderTransform(*player.local.collider, *player.local.transform);
        player.local.transform->flags = TRANSFORM::DIRTY;
    }
}

#endif //EVILENGINE_PLAYERMOVEMENT_HPP
