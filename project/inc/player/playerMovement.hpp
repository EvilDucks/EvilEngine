//
// Created by Bartek on 18.05.2024.
//

#ifndef EVILENGINE_PLAYERMOVEMENT_HPP
#define EVILENGINE_PLAYERMOVEMENT_HPP

#include "player.hpp"

namespace PLAYER::MOVEMENT {

    void Move(PLAYER::Player& player, TRANSFORM::LTransform* transforms, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders)
    {
        // Apply gravitation
        player.local.movement.velocity.y -= player.local.movement.gravitation;
        spdlog::info ("Player id {0} velocity.y {1}", player.id, player.local.movement.velocity.y);
        transforms[player.local.transformIndex].local.position.x = transforms[player.local.transformIndex].local.position.x + player.local.movement.velocity.x * player.local.movement.playerSpeed;
        transforms[player.local.transformIndex].local.position.y = transforms[player.local.transformIndex].local.position.y + player.local.movement.velocity.y * player.local.movement.playerSpeed;
        transforms[player.local.transformIndex].local.position.z = transforms[player.local.transformIndex].local.position.z + player.local.movement.velocity.z * player.local.movement.playerSpeed;

        transforms[player.local.transformIndex].flags = TRANSFORM::DIRTY;
        COLLIDER::UpdateColliderTransform(colliders[player.local.colliderGroup][player.local.colliderIndex], transforms[player.local.transformIndex]);
    }

    void Horizontal (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.movement.velocity.x = value;
    }

    void Vertical (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.movement.velocity.z = value;
    }



    void Jump (PLAYER::Player& player)
    {
        if (player.local.movement.jumpData.jumpsCount < player.local.movement.jumpData.maxJumps)
        {
            player.local.movement.velocity.y = 0;
            float v0 = 2 * player.local.movement.jumpData.jumpHeight * (1) / player.local.movement.jumpData.jumpRange;
            player.local.movement.velocity.y += v0;
            player.local.movement.jumpData.jumpsCount ++;
        }
    }
}

#endif //EVILENGINE_PLAYERMOVEMENT_HPP
