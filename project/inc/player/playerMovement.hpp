//
// Created by Bartek on 18.05.2024.
//

#ifndef EVILENGINE_PLAYERMOVEMENT_HPP
#define EVILENGINE_PLAYERMOVEMENT_HPP

#include "player.hpp"
#include "glm/gtx/rotate_vector.hpp"

namespace PLAYER::MOVEMENT {

    void ProcessMovementValue(PLAYER::Player& player)
    {
        glm::vec3 direction = glm::vec3(0.f);
        float norm = sqrt(pow(player.local.movement.movementValue.forward, 2)+pow(player.local.movement.movementValue.right, 2));
        if (norm > 0.f)
        {
            direction = glm::vec3(player.local.movement.movementValue.right/norm, 0, player.local.movement.movementValue.forward/norm);
        }

        glm::vec3 front = glm::vec3(1.f, 0.f, 0.f);
        glm::vec3 right = glm::vec3(0.f, 0.f, 1.f);
        front = glm::rotate(front, glm::radians(player.local.movement.yaw), glm::vec3(0.f, 1.f, 0.f));
        right = glm::rotate(right, glm::radians(player.local.movement.yaw), glm::vec3(0.f, 1.f, 0.f));

        player.local.movement.velocity.x = right.x * direction.x - front.x * direction.z;
        player.local.movement.velocity.z = right.z * direction.x - front.z * direction.z;

    }

    void Move(PLAYER::Player& player, TRANSFORM::LTransform* transforms, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders)
    {
        ProcessMovementValue(player);

        // Apply gravitation
        player.local.movement.velocity.y -= player.local.movement.gravitation;

        transforms[player.local.transformIndex].local.position.x = transforms[player.local.transformIndex].local.position.x + player.local.movement.velocity.x * player.local.movement.playerSpeed;
        transforms[player.local.transformIndex].local.position.y = transforms[player.local.transformIndex].local.position.y + player.local.movement.velocity.y * player.local.movement.playerSpeed;
        transforms[player.local.transformIndex].local.position.z = transforms[player.local.transformIndex].local.position.z + player.local.movement.velocity.z * player.local.movement.playerSpeed;

        transforms[player.local.transformIndex].flags = TRANSFORM::DIRTY;
        COLLIDER::UpdateColliderTransform(colliders[player.local.colliderGroup][player.local.colliderIndex], transforms[player.local.transformIndex]);
    }

    void Horizontal (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.movement.movementValue.right = value;
    }

    void Vertical (PLAYER::Player& player, float value, InputContext context)
    {
        player.local.movement.movementValue.forward = value;
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

    void ChangeDirection(PLAYER::Player& player, float yaw)
    {
        player.local.movement.yaw = -yaw;
    }


}

#endif //EVILENGINE_PLAYERMOVEMENT_HPP