//
// Created by Bartek on 02.06.2024.
//

#ifndef EVILENGINE_RIGIDBODY_HPP
#define EVILENGINE_RIGIDBODY_HPP

#include <iostream>
#include "tool/debug.hpp"
#include "render/gl.hpp"
#include "transform.hpp"
#include "force.hpp"

namespace RIGIDBODY {
    
    struct Base {
        u64 transformIndex = 0;
        u64 colliderIndex = 0;
        glm::vec3 velocity = glm::vec3(0.f);
        glm::vec3 acceleration = glm::vec3(0.f);
        float movementSpeed = 1.0f;
        float rotationSpeed = 0.5f;
        float gravitation = 0.f;
        std::vector<FORCE::Force> forces;
    };

    struct Rigidbody {
        GameObjectID id = 0;

        Base base;
    };

    void AddForce(RIGIDBODY::Rigidbody& rigidbody, glm::vec3 initialForce, float time, float acceleration = 0.f)
    {
        FORCE::Force newForce;

        glm::vec3 force = initialForce / rigidbody.base.movementSpeed;
        newForce.acceleration = 2.f * force * acceleration / (time * time);
        if (force.y > 0.f)
        {
            newForce.acceleration.y = newForce.acceleration.y + rigidbody.base.gravitation;
        }
        glm::vec3 v0 = 2.f * force / time;

        newForce.velocity = v0;

        newForce.initialForce = force;
        newForce.initialTime = time;
        newForce.totalTime = 0.f;
        rigidbody.base.forces.emplace_back(newForce);
        rigidbody.base.velocity += newForce.velocity;
    }

    void ApplyForces(RIGIDBODY::Rigidbody& rigidbody, float time)
    {
        for (int i = rigidbody.base.forces.size()-1; i > -1; i--)
        {
            auto& force = rigidbody.base.forces[i];
            glm::vec3 vk = force.velocity + force.acceleration * time;
            glm::vec3 vp = force.velocity;

            if (vk.x * vp.x < 0.f || vk.y * vp.y < 0.f || vk.z * vp.z < 0.f)
            {
                vk = glm::vec3(0.f);
            }

            rigidbody.base.velocity += vk - vp;

            // TODO: fix case when time < initialTime - totalTime
            force.velocity = vk;
            force.totalTime += time;

            if(force.totalTime >= force.initialTime)
            {
                rigidbody.base.forces.erase(rigidbody.base.forces.begin()+i);
            }
        }
    }

    void Move(RIGIDBODY::Rigidbody& rigidbody, TRANSFORM::LTransform* transforms, TRANSFORM::GTransform* gTransforms, float deltaTime)
    {
        // Apply forces
        ApplyForces (rigidbody, deltaTime);

        // Apply gravitation
        rigidbody.base.velocity.y -= rigidbody.base.gravitation * deltaTime;

        // Update position
        transforms[rigidbody.base.transformIndex].base.position += rigidbody.base.velocity * rigidbody.base.movementSpeed * deltaTime;

        // Change dirty flag
        transforms[rigidbody.base.transformIndex].flags = TRANSFORM::DIRTY;

        // Apply dirty flag
        TRANSFORM::ApplyDirtyFlagSingle(transforms[rigidbody.base.transformIndex], gTransforms[rigidbody.base.transformIndex]);

        if (rigidbody.id == 3)
        {
            std::cout << "x: " << transforms[rigidbody.base.transformIndex].base.position.x << "; y: " << transforms[rigidbody.base.transformIndex].base.position.y << "; z: " << transforms[rigidbody.base.transformIndex].base.position.z << std::endl;
        }
    }
}

#endif //EVILENGINE_RIGIDBODY_HPP
