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

void CheckOBBCollisions(COLLIDER::ColliderGroup A, COLLIDER::ColliderGroup B, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount)
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

                    glm::mat4 matB = c2.local.box.matRot * c1.local.box.matRotInverse;
                    glm::vec4 d = glm::vec4((c2.local.box.center - c1.local.box.center), 1.f);
                    glm::vec3 vPosB = c1.local.box.matRotInverse * d;

                    glm::vec3 XAxis(matB[0][0],matB[1][0],matB[2][0]);
                    glm::vec3 YAxis(matB[0][1],matB[1][1],matB[2][1]);
                    glm::vec3 ZAxis(matB[0][2],matB[1][2],matB[2][2]);


                    //15 tests
                    if (fabs(vPosB.x) > c1.local.box.bounds.x + c2.local.box.bounds.x * fabs(XAxis.x) + c2.local.box.bounds.y * fabs(XAxis.y) + c2.local.box.bounds.z * fabs(XAxis.z))
                    {break;}
                    if (fabs(vPosB.y) > c1.local.box.bounds.y + c2.local.box.bounds.x * fabs(YAxis.x) + c2.local.box.bounds.y * fabs(YAxis.y) + c2.local.box.bounds.z * fabs(YAxis.z))
                    {break;}
                    if (fabs(vPosB.z) > c1.local.box.bounds.z + c2.local.box.bounds.x * fabs(ZAxis.x) + c2.local.box.bounds.y * fabs(ZAxis.y) + c2.local.box.bounds.z * fabs(ZAxis.z))
                    {break;}
                    if (fabs(vPosB.x*XAxis.x+vPosB.y*YAxis.x+vPosB.z*ZAxis.x) > (c2.local.box.bounds.x+c1.local.box.bounds.x*fabs(XAxis.x) + c1.local.box.bounds.y * fabs(YAxis.x) + c1.local.box.bounds.z*fabs(ZAxis.x)))
                    {break;}
                    if (fabs(vPosB.x*XAxis.y+vPosB.y*YAxis.y+vPosB.z*ZAxis.y) > (c2.local.box.bounds.y+c1.local.box.bounds.x*fabs(XAxis.y) + c1.local.box.bounds.y * fabs(YAxis.y) + c1.local.box.bounds.z*fabs(ZAxis.y)))
                    {break;}
                    if (fabs(vPosB.x*XAxis.z+vPosB.y*YAxis.z+vPosB.z*ZAxis.z) > (c2.local.box.bounds.z+c1.local.box.bounds.x*fabs(XAxis.z) + c1.local.box.bounds.y * fabs(YAxis.z) + c1.local.box.bounds.z*fabs(ZAxis.z)))
                    {break;}
                    if (fabs(vPosB.z*YAxis.x-vPosB.y*ZAxis.x) > c1.local.box.bounds.y*fabs(ZAxis.x) + c1.local.box.bounds.z*fabs(YAxis.x) + c2.local.box.bounds.y*fabs(XAxis.z) + c2.local.box.bounds.z*fabs(XAxis.y))
                    {break;}
                    if (fabs(vPosB.z*YAxis.y-vPosB.y*ZAxis.y) > c1.local.box.bounds.y*fabs(ZAxis.y) + c1.local.box.bounds.z*fabs(YAxis.y) + c2.local.box.bounds.x*fabs(XAxis.z) + c2.local.box.bounds.z*fabs(XAxis.x))
                    {break;}
                    if (fabs(vPosB.z*YAxis.z-vPosB.y*ZAxis.z) > c1.local.box.bounds.y*fabs(ZAxis.z) + c1.local.box.bounds.z*fabs(YAxis.z) + c2.local.box.bounds.x*fabs(XAxis.y) + c2.local.box.bounds.y*fabs(XAxis.x))
                    {break;}
                    if (fabs(vPosB.x*ZAxis.x-vPosB.z*XAxis.x) > c1.local.box.bounds.x*fabs(ZAxis.x) + c1.local.box.bounds.z*fabs(XAxis.x) + c2.local.box.bounds.y*fabs(YAxis.z) + c2.local.box.bounds.z*fabs(YAxis.y))
                    {break;}
                    if (fabs(vPosB.x*ZAxis.y-vPosB.z*XAxis.y) > c1.local.box.bounds.x*fabs(ZAxis.y) + c1.local.box.bounds.z*fabs(XAxis.y) + c2.local.box.bounds.x*fabs(YAxis.z) + c2.local.box.bounds.z*fabs(YAxis.x))
                    {break;}
                    if (fabs(vPosB.x*ZAxis.z-vPosB.z*XAxis.z) > c1.local.box.bounds.x*fabs(ZAxis.z) + c1.local.box.bounds.z*fabs(XAxis.z) + c2.local.box.bounds.x*fabs(YAxis.y) + c2.local.box.bounds.y*fabs(YAxis.x))
                    {break;}
                    if (fabs(vPosB.y*XAxis.x-vPosB.x*YAxis.x) > c1.local.box.bounds.x*fabs(YAxis.x) + c1.local.box.bounds.y*fabs(XAxis.x) + c2.local.box.bounds.y*fabs(ZAxis.z) + c2.local.box.bounds.z*fabs(ZAxis.y))
                    {break;}
                    if (fabs(vPosB.y*XAxis.y-vPosB.x*YAxis.y) > c1.local.box.bounds.x*fabs(YAxis.y) + c1.local.box.bounds.y*fabs(XAxis.y) + c2.local.box.bounds.x*fabs(ZAxis.z) + c2.local.box.bounds.z*fabs(ZAxis.x))
                    {break;}
                    if (fabs(vPosB.y*XAxis.z-vPosB.x*YAxis.z) > c1.local.box.bounds.x*fabs(YAxis.z) + c1.local.box.bounds.y*fabs(XAxis.z) + c2.local.box.bounds.x*fabs(ZAxis.y) + c2.local.box.bounds.y*fabs(ZAxis.x))
                    {break;}
                    {
                        DEBUG {spdlog::info("OBB collision");}
                    }
                }
            }
        }
    }
}