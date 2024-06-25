//
// Created by Bartek on 18.05.2024.
//

#ifndef EVILENGINE_PLAYERMOVEMENT_HPP
#define EVILENGINE_PLAYERMOVEMENT_HPP

#include "player.hpp"
#include "playerLogic.hpp"

#include "../components/rigidbody.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "manager/audio.hpp"

namespace PLAYER::MOVEMENT {

    void CalculateGravitation(PLAYER::Player& player, RIGIDBODY::Rigidbody* rigidbodies)
    {
        rigidbodies[player.local.rigidbodyIndex].base.gravitation = 2.f * player.local.movement.jumpData.jumpHeight * pow(player.local.movement.playerSpeed, 2) / pow(player.local.movement.jumpData.jumpRange, 2);
    }

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

        player.local.movement.direction = front;

        player.local.movement.velocity.x = right.x * direction.x - front.x * direction.z;
        player.local.movement.velocity.z = right.z * direction.x - front.z * direction.z;
    }

    void SpeedPowerUp (PLAYER::Player& player)
    {
        if (player.local.movement.movementValue.forward == 0 && player.local.movement.movementValue.right == 0)
        {
            player.local.movement.movementValue.forward = -1;
        }
        else
        {
            if (player.local.movement.movementValue.forward != 0.f)
            {
                player.local.movement.movementValue.forward *= 1/fabs(player.local.movement.movementValue.forward);
            }
            if (player.local.movement.movementValue.right != 0.f)
            {
                player.local.movement.movementValue.right *= 1/fabs(player.local.movement.movementValue.right);
            }
        }
    }

    void Rotate(PLAYER::Player& player, TRANSFORM::LTransform& transform)
    {
        if (player.local.movement.velocity != glm::vec3(0.f))
        {
            transform.base.rotation.y = player.local.movement.yaw+90.f;
        }
    }

    void Move(PLAYER::Player& player, RIGIDBODY::Rigidbody* rigidbodies, float deltaTime, POWER_UP::PowerUpType powerUp, TRANSFORM::LTransform& transform)
    {
        rigidbodies[player.local.rigidbodyIndex].base.velocity -= player.local.movement.velocity;

        if (powerUp == POWER_UP::PowerUpType::SPEED)
        {
            SpeedPowerUp(player);
        }

        ProcessMovementValue(player);

        player.local.movement.movementValue.forward = 0;
        player.local.movement.movementValue.right = 0;

        rigidbodies[player.local.rigidbodyIndex].base.velocity += player.local.movement.velocity;

        // Update movement lock
        if (player.local.movement.movementLock)
        {
            player.local.movement.movementLockTimer -= deltaTime;
            if (player.local.movement.movementLockTimer < 0.f)
            {
                player.local.movement.movementLock = false;
            }
        }

        // Update charge timer and cooldown
        if (player.local.movement.chargeData.chargeTimer > 0.f)
        {
            player.local.movement.chargeData.chargeTimer -= deltaTime;
        }

        if (player.local.movement.chargeData.chargeCooldown > 0.f)
        {
            player.local.movement.chargeData.chargeCooldown -= deltaTime;
        }

        Rotate(player, transform);
    }

    void Horizontal (PLAYER::Player& player, float value, InputContext context)
    {
        if (!player.local.movement.movementLock)
        {
            player.local.movement.movementValue.right = value;
        }
        else
        {
            player.local.movement.movementValue.right = 0;
        }
    }

    void Vertical (PLAYER::Player& player, float value, InputContext context)
    {
        if (!player.local.movement.movementLock)
        {
            player.local.movement.movementValue.forward = value;
        }
        else
        {
            player.local.movement.movementValue.forward = 0;
        }
    }

    void Jump (PLAYER::Player& player, RIGIDBODY::Rigidbody* rigidbodies)
    {
        if (player.local.movement.jumpData.jumpsCount < player.local.movement.jumpData.maxJumps && !player.local.movement.movementLock)
        {
            float v0 = 2 * player.local.movement.jumpData.jumpHeight * (player.local.movement.playerSpeed) / player.local.movement.jumpData.jumpRange;
            rigidbodies[player.local.rigidbodyIndex].base.velocity.y = v0;
            player.local.movement.jumpData.jumpsCount ++;

            if (player.local.movement.jumpData.jumpsCount == 1) {
                MANAGER::AUDIO::PlaySource (MANAGER::AUDIO::SOURCES::JUMP);
            } else {
                MANAGER::AUDIO::PlaySource (MANAGER::AUDIO::SOURCES::JUMP_DOUBLE);
            }
        }
    }

    void ChangeDirection(PLAYER::Player& player, float yaw)
    {
        player.local.movement.yaw = -yaw;
    }

    void Charge(PLAYER::Player& player, RIGIDBODY::Rigidbody* rigidbodies)
    {
        if (!player.local.movement.movementLock && player.local.movement.chargeData.chargeTimer <= 0.f && player.local.movement.chargeData.chargeCooldown <= 0.f)
        {
            RIGIDBODY::ResetForce(rigidbodies[player.local.rigidbodyIndex]);
            player.local.movement.movementValue.forward = 0;
            player.local.movement.movementValue.right = 0;
            player.local.movement.chargeData.chargeTimer = player.local.movement.chargeData.duration;
            player.local.movement.chargeData.chargeDirection = player.local.movement.direction;
            PLAYER::MovementLock(player, player.local.movement.chargeData.movementLockDuration);
            RIGIDBODY::AddForce(rigidbodies[player.local.rigidbodyIndex], player.local.movement.direction, player.local.movement.chargeData.distance, player.local.movement.chargeData.duration, -1);
            player.local.movement.chargeData.chargeCooldown = player.local.movement.chargeData.chargeCooldownDuration;
        }
    }
}

#endif //EVILENGINE_PLAYERMOVEMENT_HPP
