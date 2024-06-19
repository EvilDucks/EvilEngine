#pragma once

#include "player.hpp"

#include "manager/audio.hpp"

namespace PLAYER {

    void MovementLock (PLAYER::Player& player, float timer)
    {
        player.local.movement.movementLock = true;
        player.local.movement.movementLockTimer = timer;
    }

    void Bounce (PLAYER::Player& player, RIGIDBODY::Rigidbody* rigidbodies, glm::vec3 overlap)
    {
        RIGIDBODY::ResetForce(rigidbodies[player.local.rigidbodyIndex]);
        RIGIDBODY::AddForce(rigidbodies[player.local.rigidbodyIndex], glm::normalize(overlap), POWER_UP::BOUNCE::strength, POWER_UP::BOUNCE::bounceDuration, -1.f);
    }

    void PlatformLanding (PLAYER::Player& player, RIGIDBODY::Rigidbody* rigidbodies, glm::vec3 overlap, POWER_UP::PowerUpType powerUp)
    {
        player.local.movement.jumpData.jumpsCount = 0;
        if (powerUp == POWER_UP::PowerUpType::BOUNCE && rigidbodies[player.local.rigidbodyIndex].base.velocity.y < -0.2f)
        {
            rigidbodies[player.local.rigidbodyIndex].base.velocity.y = 0;
            Bounce(player, rigidbodies, overlap);
        }
        else
        {
            rigidbodies[player.local.rigidbodyIndex].base.velocity.y = 0;
        }
    }

    void PlayerRotation (PLAYER::Player& player, float value, InputContext context, TRANSFORM::LTransform* transforms, TRANSFORM::GTransform* globalTransforms, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders)
    {
        transforms[player.local.transformIndex].base.rotation.y += value * player.local.movement.rotationSpeed;
        COLLIDER::UpdateColliderTransform(colliders[player.local.colliderGroup][player.local.colliderIndex], globalTransforms[player.local.transformIndex]);
        transforms[player.local.transformIndex].flags = TRANSFORM::DIRTY;
    }

    void PlayerPush (PLAYER::Player& player, PLAYER::Player& otherPlayer, RIGIDBODY::Rigidbody* rigidbodies)
    {
        if (player.local.movement.chargeData.chargeTimer > 0.f && otherPlayer.local.movement.chargeData.chargeTimer > 0.f)
        {
            RIGIDBODY::ResetForce(rigidbodies[otherPlayer.local.rigidbodyIndex]);
            RIGIDBODY::ResetForce(rigidbodies[player.local.rigidbodyIndex]);
            MovementLock(otherPlayer, player.local.movement.chargeData.knockbackDuration);
            MovementLock(player, player.local.movement.chargeData.knockbackDuration);
            RIGIDBODY::AddForce(rigidbodies[otherPlayer.local.rigidbodyIndex], player.local.movement.chargeData.chargeDirection, player.local.movement.chargeData.knockbackDistance, player.local.movement.chargeData.knockbackDuration, -1);
            RIGIDBODY::AddForce(rigidbodies[player.local.rigidbodyIndex], otherPlayer.local.movement.chargeData.chargeDirection, player.local.movement.chargeData.knockbackDistance, player.local.movement.chargeData.knockbackDuration, -1);
        }
        else if (player.local.movement.chargeData.chargeTimer > 0.f)
        {
            RIGIDBODY::ResetForce(rigidbodies[otherPlayer.local.rigidbodyIndex]);
            RIGIDBODY::ResetForce(rigidbodies[player.local.rigidbodyIndex]);
            MovementLock(otherPlayer, player.local.movement.chargeData.knockbackDuration);
            RIGIDBODY::AddForce(rigidbodies[otherPlayer.local.rigidbodyIndex], player.local.movement.chargeData.chargeDirection, player.local.movement.chargeData.knockbackDistance, player.local.movement.chargeData.knockbackDuration, -1);
        }
        else if (otherPlayer.local.movement.chargeData.chargeTimer > 0.f)
        {
            RIGIDBODY::ResetForce(rigidbodies[otherPlayer.local.rigidbodyIndex]);
            RIGIDBODY::ResetForce(rigidbodies[player.local.rigidbodyIndex]);
            MovementLock(player, player.local.movement.chargeData.knockbackDuration);
            RIGIDBODY::AddForce(rigidbodies[player.local.rigidbodyIndex], otherPlayer.local.movement.chargeData.chargeDirection, player.local.movement.chargeData.knockbackDistance, player.local.movement.chargeData.knockbackDuration, -1);
        }
    }

    bool MapCollision (PLAYER::Player& player, glm::vec3 overlap, TRANSFORM::LTransform* transforms, RIGIDBODY::Rigidbody* rigidbodies, POWER_UP::PowerUpType powerUp)
    {
        PROFILER { ZoneScopedN("Player: MapCollision"); }

//        if (player.local.playerIndex == 0)
//        {
//            DEBUG spdlog::info("Overlap: [{0}; {1}; {2}]", overlap.x, overlap.y, overlap.z);
//        }

        if (fabs(overlap.x) != 0.f)
        {
            transforms[player.local.transformIndex].base.position.x += overlap.x;
            RIGIDBODY::ResetForcesX(rigidbodies[player.local.rigidbodyIndex], overlap.x);
        }
        if (fabs(overlap.y) != 0.f)
        {
            if (overlap.y > 0.f)
            {
                PlatformLanding(player, rigidbodies, overlap, powerUp);
            }
            else
            {
                if (rigidbodies[player.local.rigidbodyIndex].base.velocity.y > 0)
                {
                    rigidbodies[player.local.rigidbodyIndex].base.velocity.y = 0;
                }
            }
            transforms[player.local.transformIndex].base.position.y += overlap.y;
            RIGIDBODY::ResetForcesY(rigidbodies[player.local.rigidbodyIndex], overlap.y);
        }
        if (fabs(overlap.z) != 0.f)
        {
            transforms[player.local.transformIndex].base.position.z += overlap.z;
            RIGIDBODY::ResetForcesZ(rigidbodies[player.local.rigidbodyIndex], overlap.z);
        }
        transforms[player.local.transformIndex].flags = TRANSFORM::DIRTY;
        return true;
    }

    bool PlayerCollision (PLAYER::Player& player, COLLIDER::Collider& collider, glm::vec3 overlap, TRANSFORM::LTransform* transforms, u64 transformsCount, RIGIDBODY::Rigidbody* rigidbodies, PLAYER::Player& otherPlayer, POWER_UP::PowerUpType powerUp)
    {
        PROFILER { ZoneScopedN("Player: PlayerCollision"); }

        u64 transformIndex = OBJECT::ID_DEFAULT;
        OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, transformsCount, transforms, collider.id);

        if (abs(overlap.x) != 0.f)
        {
            transforms[player.local.transformIndex].base.position.x += overlap.x * 0.5f;
            transforms[transformIndex].base.position.x -= overlap.x * 0.5f;
            RIGIDBODY::ResetForcesX(rigidbodies[player.local.rigidbodyIndex], overlap.x);
        }
        if (abs(overlap.y) != 0.f)
        {
            if (overlap.y > 0.f)
            {
                PlatformLanding(player, rigidbodies, overlap, powerUp);
            }
            else
            {
                if (rigidbodies[player.local.rigidbodyIndex].base.velocity.y > 0)
                {
                    rigidbodies[player.local.rigidbodyIndex].base.velocity.y = 0;
                }
            }
            transforms[player.local.transformIndex].base.position.y += overlap.y * 0.5f;
            transforms[transformIndex].base.position.y -= overlap.y * 0.5f;
            RIGIDBODY::ResetForcesY(rigidbodies[player.local.rigidbodyIndex], overlap.y);
        }
        if (abs(overlap.z) != 0.f)
        {
            transforms[player.local.transformIndex].base.position.z += overlap.z * 0.5f;
            transforms[transformIndex].base.position.z -= overlap.z * 0.5f;
            RIGIDBODY::ResetForcesZ(rigidbodies[player.local.rigidbodyIndex], overlap.z);
        }

        PlayerPush(player, otherPlayer, rigidbodies);

        transforms[player.local.transformIndex].flags = TRANSFORM::DIRTY;
        transforms[transformIndex].flags = TRANSFORM::DIRTY;
        return true;
    }

    void HandlePlayerCollisions (PLAYER::Player& player, std::unordered_map<COLLIDER::ColliderGroup,
                                 COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount,
                                 TRANSFORM::LTransform* transforms, TRANSFORM::GTransform* globalTransforms, u64 transformsCount,
                                 RIGIDBODY::Rigidbody* rigidbodies, PLAYER::Player& otherPlayer, POWER_UP::PowerUpType powerUp)
    {
        PROFILER { ZoneScopedN("Player: HandlePlayerCollisions"); }

        for (int i = colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList.size() - 1; i >= 0; i--)
        {
            COLLIDER::Collision _collision = colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList[i];
            bool deleteOtherCollision = true;
            switch (_collision.group){
                case COLLIDER::ColliderGroup::MAP:
                    deleteOtherCollision = MapCollision(player, _collision.overlap, transforms, rigidbodies, powerUp);
                    break;
                case COLLIDER::ColliderGroup::PLAYER:
                    deleteOtherCollision = PlayerCollision(player, colliders[COLLIDER::ColliderGroup::PLAYER][_collision.index], _collision.overlap, transforms, transformsCount, rigidbodies, otherPlayer, powerUp);
                    break;
                case COLLIDER::ColliderGroup::TRIGGER:
                    deleteOtherCollision = false;
                    break;
                default:
                    break;
            }
            auto v = colliders[_collision.group][_collision.index].local.collisionsList;
            if (deleteOtherCollision)
            {
                colliders[_collision.group][_collision.index].local.collisionsList.erase(colliders[_collision.group][_collision.index].local.collisionsList.begin() + COLLIDER::FindCollisionIndex(colliders[_collision.group][_collision.index], player.local.playerIndex, player.local.colliderGroup));
            }
            colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList.erase(colliders[player.local.colliderGroup][player.local.colliderIndex].local.collisionsList.begin() + i);
        }
    }

    void CalculateGravitation(PLAYER::Player& player, RIGIDBODY::Rigidbody* rigidbodies)
    {
        rigidbodies[player.local.rigidbodyIndex].base.gravitation = 2.f * player.local.movement.jumpData.jumpHeight * pow(player.local.movement.playerSpeed, 2) / pow(player.local.movement.jumpData.jumpRange, 2);
    }

    void UsePowerUp (PLAYER::Player& player, POWER_UP::PowerUp& activePowerUp, PLAYER::Player* players, u16 playersCount, RIGIDBODY::Rigidbody* rigidbodies)
    {
        if (activePowerUp.type == POWER_UP::PowerUpType::NONE && player.local.powerUp.type != POWER_UP::PowerUpType::NONE)
        {
            activePowerUp.type = player.local.powerUp.type;
            activePowerUp.timeLeft = player.local.powerUp.duration;
            player.local.powerUp.type = POWER_UP::PowerUpType::NONE;
            DEBUG spdlog::info("Power up start");
            MANAGER::AUDIO::PlaySource(6);

            if (activePowerUp.type == POWER_UP::PowerUpType::SPEED)
            {
                for (int i = 0; i < playersCount; i++)
                {
                    players[i].local.movement.playerSpeed *= POWER_UP::SPEED::speedMultiplier;
                    rigidbodies[players[i].local.rigidbodyIndex].base.movementSpeed = players[i].local.movement.playerSpeed;
                    CalculateGravitation(players[i], rigidbodies);
                }
            }
            else if (activePowerUp.type == POWER_UP::PowerUpType::GHOST)
            {
                for (int i = 0; i < playersCount; i++)
                {
                    players[i].local.movement.ghostForm = true;
                }
            }
        }
    }

}