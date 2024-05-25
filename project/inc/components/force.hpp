//
// Created by Bartek on 25.05.2024.
//

#ifndef EVILENGINE_FORCE_HPP
#define EVILENGINE_FORCE_HPP

#include <iostream>
#include "tool/debug.hpp"
#include "render/mesh.hpp"
#include "object.hpp"
#include "global.hpp"

namespace FORCE {

    struct Force {
        glm::vec3 initialForce;
        float initialTime;
        glm::vec3 forceLeft;
        float totalTime;
        glm::vec3 velocity;
        glm::vec3 acceleration;
        u16 transformIndex;
    };

    void AddForce(std::vector<Force>& forces, u16 transformIndex, glm::vec3 force, float time, float acceleration = 0.f)
    {
        Force newForce;
        glm::vec3 v0 = 2.f * force / time;
        newForce.velocity = v0;
        newForce.acceleration = (v0 / time - force / (time * time)) * 2.f * acceleration;
        newForce.initialForce = force;
        newForce.initialTime = time;
        newForce.totalTime = 0.f;
        newForce.transformIndex = transformIndex;
        forces.emplace_back(newForce);
    }

    void ApplyForces(std::vector<Force>& forces, TRANSFORM::LTransform* lTransforms, TRANSFORM::GTransform* gTransforms, float time)
    {
        for (int i = forces.size()-1; i > -1; i--)
        {
            auto& force = forces[i];
            glm::vec3 vk = force.velocity + force.acceleration * time;
            glm::vec3 vp = force.velocity;
            lTransforms[force.transformIndex].base.position += (vp + vk) * time / 2.f;
            TRANSFORM::ApplyDirtyFlagSingle(lTransforms[force.transformIndex], gTransforms[force.transformIndex]);

            // TODO: fix case when time < initialTime - totalTime
            force.velocity = vk;
            force.totalTime += time;
            if(force.totalTime >= force.initialTime)
            {
                forces.erase(forces.begin()+i);
            }
        }
    }
}

#endif //EVILENGINE_FORCE_HPP
