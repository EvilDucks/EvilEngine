//
// Created by Bartek on 08.04.2024.
//

#ifndef EVILENGINE_PLAYER_HPP
#define EVILENGINE_PLAYER_HPP

#include <iostream>
#include "tool/debug.hpp"
#include "render/mesh.hpp"
#include "hid/inputManager.hpp"

#include "components/rigidbody.hpp"
#include "components/powerUps/powerUp.hpp"

namespace PLAYER {

    struct JumpData {
        u8 maxJumps = 2;
        u8 jumpsCount = 0;
        float jumpRange = 3.f;
        float jumpHeight = 1.f;
    };

    struct ChargeData {
        float distance = 5.f;
        float duration = 0.5f;
        float movementLockDuration = 0.5f;
        float knockbackDistance = 7.f;
        float knockbackDuration = 0.5f;
        glm::vec3 chargeDirection = glm::vec3(0.f);
        float chargeTimer = 0.f;
        float chargeCooldown = 0.f;
        float chargeCooldownDuration = 5.f;
    };

    struct MovementValue {
        float forward;
        float right;
    };

    struct PlayerMovement {
        glm::vec3 direction = glm::vec3(1.f, 0.f, 0.f);
        MovementValue movementValue;
        float yaw = 90.f;
        glm::vec3 velocity = glm::vec3(0.f);
        float playerSpeed = 5.0f;
        float rotationSpeed = 0.5f;
        float gravitation = 0.25f;
        JumpData jumpData;
        bool movementLock = false;
        float movementLockTimer = 0.f;
        ChargeData chargeData;
        bool ghostForm = false;
    };

    struct SelectionPosition {
        unsigned int x = 0;
        unsigned int y = 0;
    };

    struct Base {
        int playerIndex = -1;
        std::vector<InputDevice> controlScheme;
        std::string name;
        u64 transformIndex = 0;
        u64 colliderIndex = 0;
        u64 rigidbodyIndex = 0;
        u64 currentCheckpointIndex = 0;
        u64 checkPointInRange = -1;
        COLLIDER::ColliderGroup colliderGroup = COLLIDER::ColliderGroup::PLAYER;
        PlayerMovement movement;
        glm::vec3 prevPosition;
        SelectionPosition selection;
        POWER_UP::PowerUp powerUp;
    };

    struct Player {
        GameObjectID id = 0;
        Base local;
    };

}

#endif //EVILENGINE_PLAYER_HPP