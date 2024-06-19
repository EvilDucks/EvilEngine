//
// Created by Bartek on 09.04.2024.
//

#ifndef EVILENGINE_COLLISIONSDETECTION_HPP
#define EVILENGINE_COLLISIONSDETECTION_HPP

#endif //EVILENGINE_COLLISIONSDETECTION_HPP

#include "collider.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "components/ui/uiManager.hpp"

void CheckCollisions(COLLIDER::ColliderGroup A, COLLIDER::ColliderGroup B, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount)
{
    PROFILER { ZoneScopedN("CollisionDetection: checkCollisions"); }

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
                        colliders[A][i].local.collisionsList.emplace_back(COLLIDER::Collision(j, B, glm::vec3(overlapSign.x * xOverlap, overlapSign.y * yOverlap, overlapSign.z * zOverlap)));
                        colliders[B][j].local.collisionsList.emplace_back(COLLIDER::Collision(i, A, glm::vec3(overlapSign.x * -xOverlap, overlapSign.y * -yOverlap, overlapSign.z * -zOverlap)));
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
        if (colliders[A][i].local.isEnabled)
        {
            for (int j = 0; j < collidersCount[B]; j++)
            {
                if (colliders[B][j].local.isEnabled)
                {
                    COLLIDER::Collider c1 = colliders[A][i];
                    COLLIDER::Collider c2 = colliders[B][j];

                    glm::mat4 matB = c2.local.box.matRot * c1.local.box.matRotInverse;
                    glm::vec4 d = glm::vec4((c2.local.box.center - c1.local.box.center), 1.f);
                    glm::vec3 vPosB = c1.local.box.matRotInverse * d;

                    glm::vec3 XAxis(matB[0][0],matB[1][0],matB[2][0]);
                    glm::vec3 YAxis(matB[0][1],matB[1][1],matB[2][1]);
                    glm::vec3 ZAxis(matB[0][2],matB[1][2],matB[2][2]);

                    float minOverlap;
                    glm::vec3 overlapAxis = glm::vec3(0.f);
                    //15 tests
                    float R_A_x1 = fabs(vPosB.x);
                    float R_A_x2 = c1.local.box.bounds.x + c2.local.box.bounds.x * fabs(XAxis.x) + c2.local.box.bounds.y * fabs(XAxis.y) + c2.local.box.bounds.z * fabs(XAxis.z);
                    if (R_A_x1 > R_A_x2)
                    {
                        continue;
                    }

                    {
                        minOverlap = fabs(R_A_x1-R_A_x2);

                        glm::vec3 aAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
                        overlapAxis = aAxis;
                    }

                    float R_A_y1 = fabs(vPosB.y);
                    float R_A_y2 = c1.local.box.bounds.y + c2.local.box.bounds.x * fabs(YAxis.x) + c2.local.box.bounds.y * fabs(YAxis.y) + c2.local.box.bounds.z * fabs(YAxis.z);
                    if (R_A_y1 > R_A_y2) {continue;}
                    float overlap = fabs(R_A_y1 - R_A_y2);
                    if ( overlap < minOverlap)
                    {
                        minOverlap = overlap;

                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                        overlapAxis = aAxis;
                    }

                    float R_A_z1 = fabs(vPosB.z);
                    float R_A_z2 = c1.local.box.bounds.z + c2.local.box.bounds.x * fabs(ZAxis.x) + c2.local.box.bounds.y * fabs(ZAxis.y) + c2.local.box.bounds.z * fabs(ZAxis.z);
                    if (R_A_z1 > R_A_z2) {continue;}
                    overlap = fabs(R_A_z1 - R_A_z2);
                    if ( overlap < minOverlap)
                    {
                        minOverlap = overlap;

                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                        overlapAxis = aAxis;
                    }

                    float R_B_x1 = fabs(vPosB.x*XAxis.x+vPosB.y*YAxis.x+vPosB.z*ZAxis.x);
                    float R_B_x2 = (c2.local.box.bounds.x+c1.local.box.bounds.x*fabs(XAxis.x) + c1.local.box.bounds.y * fabs(YAxis.x) + c1.local.box.bounds.z*fabs(ZAxis.x));
                    if (R_B_x1 > R_B_x2)
                    {continue;}
                    overlap = fabs(R_B_x1 - R_B_x2);
                    if ( overlap < minOverlap)
                    {
                        minOverlap = overlap;

                        glm::vec3 bAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                        overlapAxis = bAxis;
                    }

                    float R_B_y1 =fabs(vPosB.x*XAxis.y+vPosB.y*YAxis.y+vPosB.z*ZAxis.y);
                    float R_B_y2 =(c2.local.box.bounds.y+c1.local.box.bounds.x*fabs(XAxis.y) + c1.local.box.bounds.y * fabs(YAxis.y) + c1.local.box.bounds.z*fabs(ZAxis.y));
                    if (R_B_y1 > R_B_y2)
                    {continue;}
                    overlap = fabs(R_B_y1 - R_B_y2);
                    if ( overlap < minOverlap)
                    {
                        minOverlap = overlap;

                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                        overlapAxis = bAxis;
                    }

                    float R_B_z1 =fabs(vPosB.x*XAxis.z+vPosB.y*YAxis.z+vPosB.z*ZAxis.z);
                    float R_B_z2 =(c2.local.box.bounds.z+c1.local.box.bounds.x*fabs(XAxis.z) + c1.local.box.bounds.y * fabs(YAxis.z) + c1.local.box.bounds.z*fabs(ZAxis.z));
                    if (R_B_z1 > R_B_z2)
                    {continue;}
                    overlap = fabs(R_B_z1 - R_B_z2);
                    if ( overlap < minOverlap)
                    {
                        minOverlap = overlap;

                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                        overlapAxis = bAxis;
                    }

                    float RAxRBx1 =fabs(vPosB.z*YAxis.x-vPosB.y*ZAxis.x);
                    float RAxRBx2 =c1.local.box.bounds.y*fabs(ZAxis.x) + c1.local.box.bounds.z*fabs(YAxis.x) + c2.local.box.bounds.y*fabs(XAxis.z) + c2.local.box.bounds.z*fabs(XAxis.y);
                    if (RAxRBx1 > RAxRBx2)
                    {continue;}
//                    overlap = fabs(RAxRBx1 - RAxRBx2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAxRBy1 =fabs(vPosB.z*YAxis.y-vPosB.y*ZAxis.y);
                    float RAxRBy2 =c1.local.box.bounds.y*fabs(ZAxis.y) + c1.local.box.bounds.z*fabs(YAxis.y) + c2.local.box.bounds.x*fabs(XAxis.z) + c2.local.box.bounds.z*fabs(XAxis.x);
                    if (RAxRBy1 > RAxRBy2)
                    {continue;}
//                    overlap = fabs(RAxRBy1 - RAxRBy2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAxRBz1 =fabs(vPosB.z*YAxis.z-vPosB.y*ZAxis.z);
                    float RAxRBz2 =c1.local.box.bounds.y*fabs(ZAxis.z) + c1.local.box.bounds.z*fabs(YAxis.z) + c2.local.box.bounds.x*fabs(XAxis.y) + c2.local.box.bounds.y*fabs(XAxis.x);
                    if (RAxRBz1 > RAxRBz2)
                    {continue;}
//                    overlap = fabs(RAxRBz1 - RAxRBz2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAyRBx1 =fabs(vPosB.x*ZAxis.x-vPosB.z*XAxis.x);
                    float RAyRBx2 =c1.local.box.bounds.x*fabs(ZAxis.x) + c1.local.box.bounds.z*fabs(XAxis.x) + c2.local.box.bounds.y*fabs(YAxis.z) + c2.local.box.bounds.z*fabs(YAxis.y);
                    if (RAyRBx1 > RAyRBx2)
                    {continue;}
//                    overlap = fabs(RAyRBx1 - RAyRBx2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAyRBy1 = fabs(vPosB.x*ZAxis.y-vPosB.z*XAxis.y);
                    float RAyRBy2 = c1.local.box.bounds.x*fabs(ZAxis.y) + c1.local.box.bounds.z*fabs(XAxis.y) + c2.local.box.bounds.x*fabs(YAxis.z) + c2.local.box.bounds.z*fabs(YAxis.x);
                    if (RAyRBy1 > RAyRBy2)
                    {continue;}
//                    overlap = fabs(RAyRBy1 - RAyRBy2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAyRBz1 = fabs(vPosB.x*ZAxis.z-vPosB.z*XAxis.z);
                    float RAyRBz2 = c1.local.box.bounds.x*fabs(ZAxis.z) + c1.local.box.bounds.z*fabs(XAxis.z) + c2.local.box.bounds.x*fabs(YAxis.y) + c2.local.box.bounds.y*fabs(YAxis.x);
                    if (RAyRBz1 > RAyRBz2)
                    {continue;}
//                    overlap = fabs(RAyRBz1 - RAyRBz2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAzRBx1 = fabs(vPosB.y*XAxis.x-vPosB.x*YAxis.x);
                    float RAzRBx2 = c1.local.box.bounds.x*fabs(YAxis.x) + c1.local.box.bounds.y*fabs(XAxis.x) + c2.local.box.bounds.y*fabs(ZAxis.z) + c2.local.box.bounds.z*fabs(ZAxis.y);
                    if (RAzRBx1 > RAzRBx2)
                    {continue;}
//                    overlap = fabs(RAzRBx1 - RAzRBx2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAzRBy1 =fabs(vPosB.y*XAxis.y-vPosB.x*YAxis.y);
                    float RAzRBy2 = c1.local.box.bounds.x*fabs(YAxis.y) + c1.local.box.bounds.y*fabs(XAxis.y) + c2.local.box.bounds.x*fabs(ZAxis.z) + c2.local.box.bounds.z*fabs(ZAxis.x);
                    if (RAzRBy1 > RAzRBy2)
                    {continue;}
//                    overlap = fabs(RAzRBy1 - RAzRBy2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAzRBz1 =fabs(vPosB.y*XAxis.z-vPosB.x*YAxis.z);
                    float RAzRBz2 =c1.local.box.bounds.x*fabs(YAxis.z) + c1.local.box.bounds.y*fabs(XAxis.z) + c2.local.box.bounds.x*fabs(ZAxis.y) + c2.local.box.bounds.y*fabs(ZAxis.x);
                    if (RAzRBz1 > RAzRBz2)
                    {continue;}
//                    overlap = fabs(RAzRBz1 - RAzRBz2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    // If all true - collision detected
                    {
                        overlapAxis = glm::vec3(fabs(overlapAxis.x), fabs(overlapAxis.y), fabs(overlapAxis.z));
                        if (c1.local.box.center.x < c2.local.box.center.x) overlapAxis.x *= -1.f;
                        if (c1.local.box.center.y < c2.local.box.center.y) overlapAxis.y *= -1.f;
                        if (c1.local.box.center.z < c2.local.box.center.z) overlapAxis.z *= -1.f;
                        minOverlap += 0.000001;
                        colliders[A][i].local.collisionsList.emplace_back(COLLIDER::Collision(j, B, glm::vec3(overlapAxis.x * minOverlap, overlapAxis.y * minOverlap, overlapAxis.z * minOverlap)));
                        colliders[B][j].local.collisionsList.emplace_back(COLLIDER::Collision(i, A, glm::vec3(overlapAxis.x * -minOverlap, overlapAxis.y * -minOverlap, overlapAxis.z * -minOverlap)));
//                        if (A == COLLIDER::ColliderGroup::PLAYER && i == 0 && fabs(overlapAxis.x) * minOverlap > 0.01f)
//                        {
//                            DEBUG spdlog::info("OBB collision");
//                            DEBUG spdlog::info("Colliders y overlap: {0}", fabs(c2.local.box.yMax) - fabs(c1.local.box.yMin));
//                            DEBUG spdlog::info("Min overlap: {0}; {1}; {2}", overlapAxis.x * minOverlap, overlapAxis.y * minOverlap, overlapAxis.z * minOverlap);
//                        }

                    }
                }
            }
        }
    }
}

void CheckOBBCollisionsSingleGroup(COLLIDER::ColliderGroup A, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount)
{
    for(int i = 0; i < collidersCount[A]; i++)
    {
        if (colliders[A][i].local.isEnabled)
        {
            for (int j = i+1; j < collidersCount[A]; j++)
            {
                if (colliders[A][j].local.isEnabled) {
                    COLLIDER::Collider c1 = colliders[A][i];
                    COLLIDER::Collider c2 = colliders[A][j];

                    glm::mat4 matB = c2.local.box.matRot * c1.local.box.matRotInverse;
                    glm::vec4 d = glm::vec4((c2.local.box.center - c1.local.box.center), 1.f);
                    glm::vec3 vPosB = c1.local.box.matRotInverse * d;

                    glm::vec3 XAxis(matB[0][0], matB[1][0], matB[2][0]);
                    glm::vec3 YAxis(matB[0][1], matB[1][1], matB[2][1]);
                    glm::vec3 ZAxis(matB[0][2], matB[1][2], matB[2][2]);

                    float minOverlap;
                    glm::vec3 overlapAxis = glm::vec3(0.f);
                    //15 tests
                    float R_A_x1 = fabs(vPosB.x);
                    float R_A_x2 = c1.local.box.bounds.x + c2.local.box.bounds.x * fabs(XAxis.x) +
                                   c2.local.box.bounds.y * fabs(XAxis.y) + c2.local.box.bounds.z * fabs(XAxis.z);
                    if (R_A_x1 > R_A_x2) {
                        continue;
                    }

                    {
                        minOverlap = fabs(R_A_x1 - R_A_x2);

                        glm::vec3 aAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c1.local.box.rotation.x,
                                                      glm::vec3(1.f, 0.f, 0.f));
                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
                        overlapAxis = aAxis;
                    }

                    float R_A_y1 = fabs(vPosB.y);
                    float R_A_y2 = c1.local.box.bounds.y + c2.local.box.bounds.x * fabs(YAxis.x) +
                                   c2.local.box.bounds.y * fabs(YAxis.y) + c2.local.box.bounds.z * fabs(YAxis.z);
                    if (R_A_y1 > R_A_y2) { continue; }
                    float overlap = fabs(R_A_y1 - R_A_y2);
                    if (overlap < minOverlap) {
                        minOverlap = overlap;

                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c1.local.box.rotation.x,
                                                      glm::vec3(1.f, 0.f, 0.f));
                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                        overlapAxis = aAxis;
                    }

                    float R_A_z1 = fabs(vPosB.z);
                    float R_A_z2 = c1.local.box.bounds.z + c2.local.box.bounds.x * fabs(ZAxis.x) +
                                   c2.local.box.bounds.y * fabs(ZAxis.y) + c2.local.box.bounds.z * fabs(ZAxis.z);
                    if (R_A_z1 > R_A_z2) { continue; }
                    overlap = fabs(R_A_z1 - R_A_z2);
                    if (overlap < minOverlap) {
                        minOverlap = overlap;

                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c1.local.box.rotation.x,
                                                      glm::vec3(1.f, 0.f, 0.f));
                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                        overlapAxis = aAxis;
                    }

                    float R_B_x1 = fabs(vPosB.x * XAxis.x + vPosB.y * YAxis.x + vPosB.z * ZAxis.x);
                    float R_B_x2 = (c2.local.box.bounds.x + c1.local.box.bounds.x * fabs(XAxis.x) +
                                    c1.local.box.bounds.y * fabs(YAxis.x) + c1.local.box.bounds.z * fabs(ZAxis.x));
                    if (R_B_x1 > R_B_x2) { continue; }
                    overlap = fabs(R_B_x1 - R_B_x2);
                    if (overlap < minOverlap) {
                        minOverlap = overlap;

                        glm::vec3 bAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c2.local.box.rotation.x,
                                                      glm::vec3(1.f, 0.f, 0.f));
                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                        overlapAxis = bAxis;
                    }

                    float R_B_y1 = fabs(vPosB.x * XAxis.y + vPosB.y * YAxis.y + vPosB.z * ZAxis.y);
                    float R_B_y2 = (c2.local.box.bounds.y + c1.local.box.bounds.x * fabs(XAxis.y) +
                                    c1.local.box.bounds.y * fabs(YAxis.y) + c1.local.box.bounds.z * fabs(ZAxis.y));
                    if (R_B_y1 > R_B_y2) { continue; }
                    overlap = fabs(R_B_y1 - R_B_y2);
                    if (overlap < minOverlap) {
                        minOverlap = overlap;

                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c2.local.box.rotation.x,
                                                      glm::vec3(1.f, 0.f, 0.f));
                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                        overlapAxis = bAxis;
                    }

                    float R_B_z1 = fabs(vPosB.x * XAxis.z + vPosB.y * YAxis.z + vPosB.z * ZAxis.z);
                    float R_B_z2 = (c2.local.box.bounds.z + c1.local.box.bounds.x * fabs(XAxis.z) +
                                    c1.local.box.bounds.y * fabs(YAxis.z) + c1.local.box.bounds.z * fabs(ZAxis.z));
                    if (R_B_z1 > R_B_z2) { continue; }
                    overlap = fabs(R_B_z1 - R_B_z2);
                    if (overlap < minOverlap) {
                        minOverlap = overlap;

                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c2.local.box.rotation.x,
                                                      glm::vec3(1.f, 0.f, 0.f));
                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                        overlapAxis = bAxis;
                    }

                    float RAxRBx1 = fabs(vPosB.z * YAxis.x - vPosB.y * ZAxis.x);
                    float RAxRBx2 = c1.local.box.bounds.y * fabs(ZAxis.x) + c1.local.box.bounds.z * fabs(YAxis.x) +
                                    c2.local.box.bounds.y * fabs(XAxis.z) + c2.local.box.bounds.z * fabs(XAxis.y);
                    if (RAxRBx1 > RAxRBx2) { continue; }
//                    overlap = fabs(RAxRBx1 - RAxRBx2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAxRBy1 = fabs(vPosB.z * YAxis.y - vPosB.y * ZAxis.y);
                    float RAxRBy2 = c1.local.box.bounds.y * fabs(ZAxis.y) + c1.local.box.bounds.z * fabs(YAxis.y) +
                                    c2.local.box.bounds.x * fabs(XAxis.z) + c2.local.box.bounds.z * fabs(XAxis.x);
                    if (RAxRBy1 > RAxRBy2) { continue; }
//                    overlap = fabs(RAxRBy1 - RAxRBy2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAxRBz1 = fabs(vPosB.z * YAxis.z - vPosB.y * ZAxis.z);
                    float RAxRBz2 = c1.local.box.bounds.y * fabs(ZAxis.z) + c1.local.box.bounds.z * fabs(YAxis.z) +
                                    c2.local.box.bounds.x * fabs(XAxis.y) + c2.local.box.bounds.y * fabs(XAxis.x);
                    if (RAxRBz1 > RAxRBz2) { continue; }
//                    overlap = fabs(RAxRBz1 - RAxRBz2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAyRBx1 = fabs(vPosB.x * ZAxis.x - vPosB.z * XAxis.x);
                    float RAyRBx2 = c1.local.box.bounds.x * fabs(ZAxis.x) + c1.local.box.bounds.z * fabs(XAxis.x) +
                                    c2.local.box.bounds.y * fabs(YAxis.z) + c2.local.box.bounds.z * fabs(YAxis.y);
                    if (RAyRBx1 > RAyRBx2) { continue; }
//                    overlap = fabs(RAyRBx1 - RAyRBx2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAyRBy1 = fabs(vPosB.x * ZAxis.y - vPosB.z * XAxis.y);
                    float RAyRBy2 = c1.local.box.bounds.x * fabs(ZAxis.y) + c1.local.box.bounds.z * fabs(XAxis.y) +
                                    c2.local.box.bounds.x * fabs(YAxis.z) + c2.local.box.bounds.z * fabs(YAxis.x);
                    if (RAyRBy1 > RAyRBy2) { continue; }
//                    overlap = fabs(RAyRBy1 - RAyRBy2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAyRBz1 = fabs(vPosB.x * ZAxis.z - vPosB.z * XAxis.z);
                    float RAyRBz2 = c1.local.box.bounds.x * fabs(ZAxis.z) + c1.local.box.bounds.z * fabs(XAxis.z) +
                                    c2.local.box.bounds.x * fabs(YAxis.y) + c2.local.box.bounds.y * fabs(YAxis.x);
                    if (RAyRBz1 > RAyRBz2) { continue; }
//                    overlap = fabs(RAyRBz1 - RAyRBz2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAzRBx1 = fabs(vPosB.y * XAxis.x - vPosB.x * YAxis.x);
                    float RAzRBx2 = c1.local.box.bounds.x * fabs(YAxis.x) + c1.local.box.bounds.y * fabs(XAxis.x) +
                                    c2.local.box.bounds.y * fabs(ZAxis.z) + c2.local.box.bounds.z * fabs(ZAxis.y);
                    if (RAzRBx1 > RAzRBx2) { continue; }
//                    overlap = fabs(RAzRBx1 - RAzRBx2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAzRBy1 = fabs(vPosB.y * XAxis.y - vPosB.x * YAxis.y);
                    float RAzRBy2 = c1.local.box.bounds.x * fabs(YAxis.y) + c1.local.box.bounds.y * fabs(XAxis.y) +
                                    c2.local.box.bounds.x * fabs(ZAxis.z) + c2.local.box.bounds.z * fabs(ZAxis.x);
                    if (RAzRBy1 > RAzRBy2) { continue; }
//                    overlap = fabs(RAzRBy1 - RAzRBy2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    float RAzRBz1 = fabs(vPosB.y * XAxis.z - vPosB.x * YAxis.z);
                    float RAzRBz2 = c1.local.box.bounds.x * fabs(YAxis.z) + c1.local.box.bounds.y * fabs(XAxis.z) +
                                    c2.local.box.bounds.x * fabs(ZAxis.y) + c2.local.box.bounds.y * fabs(ZAxis.x);
                    if (RAzRBz1 > RAzRBz2) { continue; }
//                    overlap = fabs(RAzRBz1 - RAzRBz2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

                    // If all true - collision detected
                    {
                        //DEBUG spdlog::info("OBB collision");
                        overlapAxis = glm::vec3(fabs(overlapAxis.x), fabs(overlapAxis.y), fabs(overlapAxis.z));
                        if (c1.local.box.center.x < c2.local.box.center.x) overlapAxis.x *= -1.f;
                        if (c1.local.box.center.y < c2.local.box.center.y) overlapAxis.y *= -1.f;
                        if (c1.local.box.center.z < c2.local.box.center.z) overlapAxis.z *= -1.f;
                        minOverlap += 0.000001;
                        colliders[A][i].local.collisionsList.emplace_back(COLLIDER::Collision(j, A, glm::vec3(
                                overlapAxis.x * minOverlap, overlapAxis.y * minOverlap, overlapAxis.z * minOverlap)));
                        colliders[A][j].local.collisionsList.emplace_back(COLLIDER::Collision(i, A, glm::vec3(
                                overlapAxis.x * -minOverlap, overlapAxis.y * -minOverlap,
                                overlapAxis.z * -minOverlap)));
                    }
                }
            }
        }
    }
}

void CheckOBBCollisionsSingleCollider(const COLLIDER::Collider& singleCollider, COLLIDER::ColliderGroup B, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount, glm::vec3& overlapVector)
{
    for (int j = 0; j < collidersCount[B]; j++)
    {
        if (colliders[B][j].local.group == B)
        {
            COLLIDER::Collider c1 = singleCollider;
            COLLIDER::Collider c2 = colliders[B][j];

            glm::mat4 matB = c2.local.box.matRot * c1.local.box.matRotInverse;
            glm::vec4 d = glm::vec4((c2.local.box.center - c1.local.box.center), 1.f);
            glm::vec3 vPosB = c1.local.box.matRotInverse * d;

            glm::vec3 XAxis(matB[0][0],matB[1][0],matB[2][0]);
            glm::vec3 YAxis(matB[0][1],matB[1][1],matB[2][1]);
            glm::vec3 ZAxis(matB[0][2],matB[1][2],matB[2][2]);

            float minOverlap;
            glm::vec3 overlapAxis = glm::vec3(0.f);
            //15 tests
            float R_A_x1 = fabs(vPosB.x);
            float R_A_x2 = c1.local.box.bounds.x + c2.local.box.bounds.x * fabs(XAxis.x) + c2.local.box.bounds.y * fabs(XAxis.y) + c2.local.box.bounds.z * fabs(XAxis.z);
            if (R_A_x1 > R_A_x2)
            {
                continue;
            }

            {
                minOverlap = fabs(R_A_x1-R_A_x2);

                glm::vec3 aAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
                aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
                overlapAxis = aAxis;
            }

            float R_A_y1 = fabs(vPosB.y);
            float R_A_y2 = c1.local.box.bounds.y + c2.local.box.bounds.x * fabs(YAxis.x) + c2.local.box.bounds.y * fabs(YAxis.y) + c2.local.box.bounds.z * fabs(YAxis.z);
            if (R_A_y1 > R_A_y2) {continue;}
            float overlap = fabs(R_A_y1 - R_A_y2);
            if ( overlap < minOverlap)
            {
                minOverlap = overlap;

                glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
                aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                overlapAxis = aAxis;
            }

            float R_A_z1 = fabs(vPosB.z);
            float R_A_z2 = c1.local.box.bounds.z + c2.local.box.bounds.x * fabs(ZAxis.x) + c2.local.box.bounds.y * fabs(ZAxis.y) + c2.local.box.bounds.z * fabs(ZAxis.z);
            if (R_A_z1 > R_A_z2) {continue;}
            overlap = fabs(R_A_z1 - R_A_z2);
            if ( overlap < minOverlap)
            {
                minOverlap = overlap;

                glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
                aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                overlapAxis = aAxis;
            }

            float R_B_x1 = fabs(vPosB.x*XAxis.x+vPosB.y*YAxis.x+vPosB.z*ZAxis.x);
            float R_B_x2 = (c2.local.box.bounds.x+c1.local.box.bounds.x*fabs(XAxis.x) + c1.local.box.bounds.y * fabs(YAxis.x) + c1.local.box.bounds.z*fabs(ZAxis.x));
            if (R_B_x1 > R_B_x2)
            {continue;}
            overlap = fabs(R_B_x1 - R_B_x2);
            if ( overlap < minOverlap)
            {
                minOverlap = overlap;

                glm::vec3 bAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
                bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                overlapAxis = bAxis;
            }

            float R_B_y1 =fabs(vPosB.x*XAxis.y+vPosB.y*YAxis.y+vPosB.z*ZAxis.y);
            float R_B_y2 =(c2.local.box.bounds.y+c1.local.box.bounds.x*fabs(XAxis.y) + c1.local.box.bounds.y * fabs(YAxis.y) + c1.local.box.bounds.z*fabs(ZAxis.y));
            if (R_B_y1 > R_B_y2)
            {continue;}
            overlap = fabs(R_B_y1 - R_B_y2);
            if ( overlap < minOverlap)
            {
                minOverlap = overlap;

                glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
                bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                overlapAxis = bAxis;
            }

            float R_B_z1 =fabs(vPosB.x*XAxis.z+vPosB.y*YAxis.z+vPosB.z*ZAxis.z);
            float R_B_z2 =(c2.local.box.bounds.z+c1.local.box.bounds.x*fabs(XAxis.z) + c1.local.box.bounds.y * fabs(YAxis.z) + c1.local.box.bounds.z*fabs(ZAxis.z));
            if (R_B_z1 > R_B_z2)
            {continue;}
            overlap = fabs(R_B_z1 - R_B_z2);
            if ( overlap < minOverlap)
            {
                minOverlap = overlap;

                glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
                bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
                bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));

                overlapAxis = bAxis;
            }

            float RAxRBx1 =fabs(vPosB.z*YAxis.x-vPosB.y*ZAxis.x);
            float RAxRBx2 =c1.local.box.bounds.y*fabs(ZAxis.x) + c1.local.box.bounds.z*fabs(YAxis.x) + c2.local.box.bounds.y*fabs(XAxis.z) + c2.local.box.bounds.z*fabs(XAxis.y);
            if (RAxRBx1 > RAxRBx2)
            {continue;}
//                    overlap = fabs(RAxRBx1 - RAxRBx2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

            float RAxRBy1 =fabs(vPosB.z*YAxis.y-vPosB.y*ZAxis.y);
            float RAxRBy2 =c1.local.box.bounds.y*fabs(ZAxis.y) + c1.local.box.bounds.z*fabs(YAxis.y) + c2.local.box.bounds.x*fabs(XAxis.z) + c2.local.box.bounds.z*fabs(XAxis.x);
            if (RAxRBy1 > RAxRBy2)
            {continue;}
//                    overlap = fabs(RAxRBy1 - RAxRBy2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

            float RAxRBz1 =fabs(vPosB.z*YAxis.z-vPosB.y*ZAxis.z);
            float RAxRBz2 =c1.local.box.bounds.y*fabs(ZAxis.z) + c1.local.box.bounds.z*fabs(YAxis.z) + c2.local.box.bounds.x*fabs(XAxis.y) + c2.local.box.bounds.y*fabs(XAxis.x);
            if (RAxRBz1 > RAxRBz2)
            {continue;}
//                    overlap = fabs(RAxRBz1 - RAxRBz2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

            float RAyRBx1 =fabs(vPosB.x*ZAxis.x-vPosB.z*XAxis.x);
            float RAyRBx2 =c1.local.box.bounds.x*fabs(ZAxis.x) + c1.local.box.bounds.z*fabs(XAxis.x) + c2.local.box.bounds.y*fabs(YAxis.z) + c2.local.box.bounds.z*fabs(YAxis.y);
            if (RAyRBx1 > RAyRBx2)
            {continue;}
//                    overlap = fabs(RAyRBx1 - RAyRBx2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

            float RAyRBy1 = fabs(vPosB.x*ZAxis.y-vPosB.z*XAxis.y);
            float RAyRBy2 = c1.local.box.bounds.x*fabs(ZAxis.y) + c1.local.box.bounds.z*fabs(XAxis.y) + c2.local.box.bounds.x*fabs(YAxis.z) + c2.local.box.bounds.z*fabs(YAxis.x);
            if (RAyRBy1 > RAyRBy2)
            {continue;}
//                    overlap = fabs(RAyRBy1 - RAyRBy2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

            float RAyRBz1 = fabs(vPosB.x*ZAxis.z-vPosB.z*XAxis.z);
            float RAyRBz2 = c1.local.box.bounds.x*fabs(ZAxis.z) + c1.local.box.bounds.z*fabs(XAxis.z) + c2.local.box.bounds.x*fabs(YAxis.y) + c2.local.box.bounds.y*fabs(YAxis.x);
            if (RAyRBz1 > RAyRBz2)
            {continue;}
//                    overlap = fabs(RAyRBz1 - RAyRBz2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

            float RAzRBx1 = fabs(vPosB.y*XAxis.x-vPosB.x*YAxis.x);
            float RAzRBx2 = c1.local.box.bounds.x*fabs(YAxis.x) + c1.local.box.bounds.y*fabs(XAxis.x) + c2.local.box.bounds.y*fabs(ZAxis.z) + c2.local.box.bounds.z*fabs(ZAxis.y);
            if (RAzRBx1 > RAzRBx2)
            {continue;}
//                    overlap = fabs(RAzRBx1 - RAzRBx2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(1.f, 0.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

            float RAzRBy1 =fabs(vPosB.y*XAxis.y-vPosB.x*YAxis.y);
            float RAzRBy2 = c1.local.box.bounds.x*fabs(YAxis.y) + c1.local.box.bounds.y*fabs(XAxis.y) + c2.local.box.bounds.x*fabs(ZAxis.z) + c2.local.box.bounds.z*fabs(ZAxis.x);
            if (RAzRBy1 > RAzRBy2)
            {continue;}
//                    overlap = fabs(RAzRBy1 - RAzRBy2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 1.f, 0.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

            float RAzRBz1 =fabs(vPosB.y*XAxis.z-vPosB.x*YAxis.z);
            float RAzRBz2 =c1.local.box.bounds.x*fabs(YAxis.z) + c1.local.box.bounds.y*fabs(XAxis.z) + c2.local.box.bounds.x*fabs(ZAxis.y) + c2.local.box.bounds.y*fabs(ZAxis.x);
            if (RAzRBz1 > RAzRBz2)
            {continue;}
//                    overlap = fabs(RAzRBz1 - RAzRBz2);
//                    if ( overlap < minOverlap)
//                    {
//                        minOverlap = overlap;
//
//                        glm::vec3 aAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c1.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        aAxis = glm::rotate(aAxis, c1.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        glm::vec3 bAxis = glm::rotate(glm::vec3(0.f, 0.f, 1.f), c2.local.box.rotation.x, glm::vec3(1.f, 0.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.y, glm::vec3(0.f, 1.f, 0.f));
//                        bAxis = glm::rotate(bAxis, c2.local.box.rotation.z, glm::vec3(0.f, 0.f, 1.f));
//
//                        overlapAxis = glm::cross(aAxis, bAxis);
//                    }

            // If all true - collision detected
            {


                if (c1.local.box.center.x < c2.local.box.center.x) overlapAxis.x *= -1.f;
                if (c1.local.box.center.y < c2.local.box.center.y) overlapAxis.y *= -1.f;
                if (c1.local.box.center.z < c2.local.box.center.z) overlapAxis.z *= -1.f;
                minOverlap += 0.000001;
//                        colliders[A][i].local.collisionsList.emplace_back(COLLIDER::Collision(j, B, glm::vec3(overlapAxis.x * minOverlap, overlapAxis.y * minOverlap, overlapAxis.z * minOverlap)));
//                        colliders[B][j].local.collisionsList.emplace_back(COLLIDER::Collision(i, A, glm::vec3(overlapAxis.x * -minOverlap, overlapAxis.y * -minOverlap, overlapAxis.z * -minOverlap)));


                overlapVector = glm::vec3(overlapAxis.x * -minOverlap, overlapAxis.y * -minOverlap, overlapAxis.z * -minOverlap);
            }
        }
    }
}

void CheckUICollisions(COLLIDER::Collider* colliders, u64 collidersCount, unsigned int mouseX, unsigned int mouseY, UI::BUTTON::Button* buttons, u16 buttonsCount, UI::MANAGER::UIM manager)
{
    for (int i = 0; i < buttonsCount; i++)
    {
        buttons[i].local.state = 0;
    }

    manager->currentHoverIndex = -1;
    manager->currentHoverType = UI::ElementType::UNKNOWN;

    for (int i = 0; i < collidersCount; i++)
    {
        if (mouseX > colliders[i].local.box.xMin && mouseX < colliders[i].local.box.xMax && mouseY > colliders[i].local.box.yMin && mouseY < colliders[i].local.box.yMax)
        {
            	u64 buttonIndex = 0;
            	OBJECT::GetComponentFast<UI::BUTTON::Button>(buttonIndex, buttonsCount, buttons, colliders[i].id);
                buttons[buttonIndex].local.state = 1;
                manager->currentHoverIndex = buttonIndex;
                manager->currentHoverType = UI::ElementType::BUTTON;
        }
    }

}