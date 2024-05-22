//
// Created by Bartek on 18.05.2024.
//

#ifndef EVILENGINE_PLAYERMOVEMENT_HPP
#define EVILENGINE_PLAYERMOVEMENT_HPP

#include "player.hpp"

namespace PLAYER::MOVEMENT {

    void Move(PLAYER::Player& player, TRANSFORM::LTransform* transforms, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders)
    {
//        if (player.id == 3)
//        {
//            spdlog::info ("Player id {0} direction {1}; {2}; {3}", player.id, player.local.movement.direction.x, player.local.movement.direction.y, player.local.movement.direction.z);
//        }

        if (player.id == 3)
        {
            spdlog::info ("Player id {0} velocity {1}; {2}; {3}", player.id, player.local.movement.velocity.x, player.local.movement.velocity.y, player.local.movement.velocity.z);
        }

        // Apply gravitation
        player.local.movement.velocity.y -= player.local.movement.gravitation;
//        spdlog::info ("Player id {0} velocity.y {1}", player.id, player.local.movement.velocity.y);
        transforms[player.local.transformIndex].local.position.x = transforms[player.local.transformIndex].local.position.x + player.local.movement.velocity.x * player.local.movement.playerSpeed;
        transforms[player.local.transformIndex].local.position.y = transforms[player.local.transformIndex].local.position.y + player.local.movement.velocity.y * player.local.movement.playerSpeed;
        transforms[player.local.transformIndex].local.position.z = transforms[player.local.transformIndex].local.position.z + player.local.movement.velocity.z * player.local.movement.playerSpeed;

        transforms[player.local.transformIndex].flags = TRANSFORM::DIRTY;
        COLLIDER::UpdateColliderTransform(colliders[player.local.colliderGroup][player.local.colliderIndex], transforms[player.local.transformIndex]);
    }

    void Horizontal (PLAYER::Player& player, float value, InputContext context)
    {
        if (player.id == 3)
        {
            spdlog::info ("Player id {0} direction {1}; {2}; {3}", player.id, player.local.movement.front.x, player.local.movement.front.y, player.local.movement.front.z);
        }
        player.local.movement.velocity.x = value * player.local.movement.right.x;
        player.local.movement.velocity.z = value * player.local.movement.right.z;
    }

    void Vertical (PLAYER::Player& player, float value, InputContext context)
    {
        if (player.id == 3)
        {
            spdlog::info ("Player id {0} direction {1}; {2}; {3}", player.id, player.local.movement.front.x, player.local.movement.front.y, player.local.movement.front.z);
        }
        player.local.movement.velocity.x = value * player.local.movement.front.x;
        player.local.movement.velocity.z = value * player.local.movement.front.z;
    }



    void Jump (PLAYER::Player& player)
    {
        if (player.local.movement.jumpData.jumpsCount < player.local.movement.jumpData.maxJumps)
        {
            player.local.movement.velocity.y = 0;
            float v0 = 2 * player.local.movement.jumpData.jumpHeight * (player.local.movement.playerSpeed) / player.local.movement.jumpData.jumpRange;
            player.local.movement.velocity.y += v0;
            player.local.movement.jumpData.jumpsCount ++;
        }
    }

    void CalculateGravitation(PLAYER::Player& player)
    {
        player.local.movement.gravitation = 2.f * player.local.movement.jumpData.jumpHeight * pow(player.local.movement.playerSpeed, 2) / pow(player.local.movement.jumpData.jumpRange, 2);
    }

    void ChangeDirection(PLAYER::Player& player, glm::vec3 front, glm::vec3 right)
    {
        player.local.movement.front = -front;
        player.local.movement.right = right;
    }
}

#endif //EVILENGINE_PLAYERMOVEMENT_HPP
