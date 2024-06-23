#pragma once

#include "player/player.hpp"
#include "player/playerMovement.hpp"

#include "manager/audio.hpp"

namespace AGENT {

    enum StateType{Wait, WindUp, Active, Recharge, Inactive};

    struct Knockback {
        float horizontalToHeightRatio = 2.f;
        float strength = 15.f;
        float duration = 0.5f;
        float movementLockDuration = 1.5f;
    };

    //opening window trap
    struct WindowData {
        GameObjectID id = 0;
        GameObjectID colliderId = 0;
        GameObjectID transformId = 0;
        GameObjectID knockbackTriggerId = 0;
        StateType type = Inactive;
        bool isActive = false;
        bool isRechargable = false;
        bool isTriggered = false;
        GameObjectID parentId = 0;
        float timer = -1.f;
        //TEMP
        glm::vec3 newRot{};
        Knockback knockback;
        bool applyKnockback = false;
    };

    void ChangeState(WindowData& window, StateType newType)
    {
        window.type = newType;
    }

    void Knockback (COLLIDER::Collider collider, PLAYER::Player& player, RIGIDBODY::Rigidbody* rigidbodies)
    {
        player.local.movement.movementValue.forward = 0;
        player.local.movement.movementValue.right = 0;
        RIGIDBODY::ResetForce(rigidbodies[player.local.rigidbodyIndex]);

        WindowData windowData;
        PLAYER::MovementLock(player, windowData.knockback.movementLockDuration);

        glm::vec3 direction = (collider.local.box.center - glm::vec3(0.f, collider.local.box.center.y, -25.f));

        direction = glm::normalize(direction);
        direction *= windowData.knockback.horizontalToHeightRatio;
        direction.y = 1.f;
        direction = glm::normalize(direction);

        //DEBUG spdlog::info("Direction: [{0}; {1}; {2}]", direction.x, direction.y, direction.z);

        RIGIDBODY::AddForce(rigidbodies[player.local.rigidbodyIndex], direction, windowData.knockback.strength, windowData.knockback.duration, -1);

        // TODO: chamge to another sound
        MANAGER::AUDIO::PlaySource (0);
    }

}