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
                        colliders[A][i].local.collision = true;
                        colliders[B][j].local.collision = true;
                    }

                }
            }
        }

    }
}