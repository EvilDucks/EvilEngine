//
// Created by Bartek on 09.04.2024.
//

#ifndef EVILENGINE_COLLISIONSDETECTION_HPP
#define EVILENGINE_COLLISIONSDETECTION_HPP

#endif //EVILENGINE_COLLISIONSDETECTION_HPP

#include "collider.hpp"

void CheckCollisions(COLLIDER::ColliderGroup A, COLLIDER::ColliderGroup B, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount)
{
    for(int i = 0; i < collidersCount[A]; i++)
    {
        if (colliders[A][i].local.group == A)
        {
            for (int j = 0; j < collidersCount[B]; j++)
            {
                if (colliders[B][j].local.group == B)
                {
                    COLLIDER::Collider c1 = colliders[A][i];
                    COLLIDER::Collider c2 = colliders[B][j];

                    if (
                    c1.local.box.xMin <= c2.local.box.xMax &&
                    c1.local.box.xMax >= c2.local.box.xMin &&
                    c1.local.box.yMin <= c2.local.box.yMax &&
                    c1.local.box.yMax >= c2.local.box.yMin &&
                    c1.local.box.zMin <= c2.local.box.zMax &&
                    c1.local.box.zMax >= c2.local.box.zMin
                            )
                    {
                        DEBUG {spdlog::info("collision");}
                        float xOverlap = fabs(std::max(c1.local.box.xMin, c2.local.box.xMin) - std::min(c1.local.box.xMax, c2.local.box.xMax));
                        float yOverlap = fabs(std::max(c1.local.box.yMin, c2.local.box.yMin) - std::min(c1.local.box.yMax, c2.local.box.yMax));
                        float zOverlap = fabs(std::max(c1.local.box.zMin, c2.local.box.zMin) - std::min(c1.local.box.zMax, c2.local.box.zMax));
                        glm::vec3 overlapSign = glm::vec3(1.f);
                        // maybe switch to checking centers?
                        if (c1.local.box.xMin < c2.local.box.xMin) overlapSign.x = -1.f;
                        if (c1.local.box.yMin < c2.local.box.yMin) overlapSign.y = -1.f;
                        if (c1.local.box.zMin < c2.local.box.zMin) overlapSign.z = -1.f;
                        colliders[A][i].local.collisionsList.emplace_back(COLLIDER::Collision(colliders[B][j].id, B, glm::vec3(overlapSign.x * xOverlap, overlapSign.y * yOverlap, overlapSign.z * zOverlap)));
                        colliders[B][j].local.collisionsList.emplace_back(COLLIDER::Collision(colliders[A][i].id, A, glm::vec3(overlapSign.x * -xOverlap, overlapSign.y * -yOverlap, overlapSign.z * -zOverlap)));
                    }
                }
            }
        }
    }
}