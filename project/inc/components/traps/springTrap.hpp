//
// Created by Bartek on 10.06.2024.
//

#ifndef EVILENGINE_SPRINGTRAP_HPP
#define EVILENGINE_SPRINGTRAP_HPP

#include <iostream>
#include "tool/debug.hpp"
#include "../../player/player.hpp"

namespace SPRING_TRAP {

    struct Knockback {
        float horizontalToHeightRatio = 1.f;
        float strength = 15.f;
        float duration = 0.5f;
        float movementLockDuration = 1.5f;
    };

    struct SpringTrap {
        Knockback knockback;
    };

    void Activation(COLLIDER::Collider collider, PLAYER::Player& player, TRANSFORM::LTransform* transforms, RIGIDBODY::Rigidbody* rigidbodies)
    {
        player.local.movement.movementValue.forward = 0;
        player.local.movement.movementValue.right = 0;
        RIGIDBODY::ResetForce(rigidbodies[player.local.rigidbodyIndex]);

        SpringTrap springTrap;
        PLAYER::MovementLock(player, springTrap.knockback.movementLockDuration);

        glm::vec3 direction = (collider.local.box.center - glm::vec3(0.f, collider.local.box.center.y, -25.f));

        direction = glm::normalize(direction);
        direction *= springTrap.knockback.horizontalToHeightRatio;
        direction.y = 1.f;
        direction = glm::normalize(direction);

        DEBUG spdlog::info("Direction: [{0}; {1}; {2}]", direction.x, direction.y, direction.z);

        RIGIDBODY::AddForce(rigidbodies[player.local.rigidbodyIndex], direction, springTrap.knockback.strength, springTrap.knockback.duration, -1);


    }

}

#endif //EVILENGINE_SPRINGTRAP_HPP
