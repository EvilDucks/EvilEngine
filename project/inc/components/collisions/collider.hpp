//
// Created by Bartek on 05.04.2024.
//

#ifndef EVILENGINE_COLLIDER_HPP
#define EVILENGINE_COLLIDER_HPP

#endif //EVILENGINE_COLLIDER_HPP

#pragma once

#include <iostream>
#include "tool/debug.hpp"
#include "render/mesh.hpp"
#include "object.hpp"
#include "global.hpp"
#include "../ui/rect.hpp"
#include <glm/gtx/matrix_decompose.hpp>

namespace COLLIDER {
    using Scale = glm::vec3;
    using IsTrigger = bool;

    enum class ColliderType : u8 {
        AABB,
        SPHERE,
        OBB,
        OBB2,
        PLANE,
    };

    enum class ColliderGroup : u8  {
        PLAYER,
        MAP,
        HAZARDS,
        UI,
        TRIGGER,
        CAMERA,
    };

    struct BoundingBox {
        float xMin = 0;
        float xMax = 0;
        float yMin = 0;
        float yMax = 0;
        float zMin = 0;
        float zMax = 0;
        glm::vec3 center = glm::vec3(0.f);
        glm::vec3 bounds = glm::vec3(1.f);
        glm::mat4 matRot = glm::mat4(1.f);
        glm::mat4 matRotInverse = glm::mat4(1.f);
        glm::vec3 rotation = glm::vec3(0.f);
    };

    struct Collision {
        u16 index;
        ColliderGroup group;
        glm::vec3 overlap = glm::vec3(0.f);
    };

    struct Base {
        Scale size = glm::vec3(1.f);
        IsTrigger isTrigger;
        ColliderType type;
        ColliderGroup group;
        bool isEnabled = true;
        BoundingBox box;
        std::vector<Collision> collisionsList;
        std::string collisionEventName;
        u16 transformIndex;
        u16 segmentIndex;
    };

    struct Collider {
        GameObjectID id = 0;
        Base local;
    };

    glm::mat3 decomposeMtx(const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale)
    {
        pos = m[3];
        for(int i = 0; i < 3; i++)
            scale[i] = glm::length(glm::vec3(m[i]));
        const glm::mat3 rotMtx(
                glm::vec3(m[0]) / scale[0],
                glm::vec3(m[1]) / scale[1],
                glm::vec3(m[2]) / scale[2]);
        rot = glm::quat_cast(rotMtx);
        return rotMtx;
    }

    void InitializeColliderSize(Collider& collider, MESH::Mesh& mesh, glm::mat4 globalTransform) {
        PROFILER { ZoneScopedN("Collider: InitializeColliderSize"); }

        //DEBUG spdlog::info ("collider.size: {0}, {1}, {2}", collider.local.size.x, collider.local.size.y, collider.local.size.z);
        //DEBUG spdlog::info (
        //    "bounds: {0}, {1}, {2}, {3}, {4}, {5}",
        //    mesh.base.boundsMin.x, mesh.base.boundsMax.x, mesh.base.boundsMin.y, 
        //    mesh.base.boundsMax.y, mesh.base.boundsMin.z, mesh.base.boundsMax.z
        //);
        //
        //// assuming meshes are in interval from -x to x
        //collider.local.size = glm::vec3((abs(mesh.base.boundsMin.x) + abs(mesh.base.boundsMax.x))/2.f, (abs(mesh.base.boundsMin.y) + abs(mesh.base.boundsMax.y))/2.f, (abs(mesh.base.boundsMin.z) + abs(mesh.base.boundsMax.z))/2.f);
        //DEBUG spdlog::info ("collider.size: {0}, {1}, {2}", collider.local.size.x, collider.local.size.y, collider.local.size.z);

        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rot;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::mat3 rotation = decomposeMtx(globalTransform, position, rot, scale);
        //rotation = glm::conjugate(rotation);

        collider.local.box.bounds = glm::vec3(collider.local.size.x * scale.x, collider.local.size.y * scale.y, collider.local.size.z * scale.z);
        collider.local.box.center = position;

        collider.local.box.xMax = collider.local.box.bounds.x + collider.local.box.center.x;
        collider.local.box.xMin = -collider.local.box.bounds.x + collider.local.box.center.x;

        collider.local.box.yMax = collider.local.box.bounds.y + collider.local.box.center.y;
        collider.local.box.yMin = -collider.local.box.bounds.y + collider.local.box.center.y;

        collider.local.box.zMax = collider.local.box.bounds.z + collider.local.box.center.z;
        collider.local.box.zMin = -collider.local.box.bounds.z + collider.local.box.center.z;

        glm::mat4 rotationMatrix = glm::mat4(1.f);
        if (collider.local.type == ColliderType::OBB)
        {
            rotationMatrix = rotationMatrix * glm::mat4(rotation);
//            glm::vec3 euler = glm::eulerAngles(rotation);
//            collider.local.box.rotation = euler;
//            rotationMatrix = glm::rotate (rotationMatrix, euler.x, glm::vec3 (1.0f, 0.0f, 0.0f));
//            rotationMatrix = glm::rotate (rotationMatrix, euler.y, glm::vec3 (0.0f, 1.0f, 0.0f));
//            rotationMatrix = glm::rotate (rotationMatrix, euler.z, glm::vec3 (0.0f, 0.0f, 1.0f));
        }
        else if (collider.local.type == ColliderType::OBB2)
        {
            rotationMatrix = rotationMatrix * glm::mat4(rotation);
            glm::vec3 euler = glm::eulerAngles(rot);
            collider.local.box.rotation.y = euler.y;
//            rotationMatrix = glm::rotate (rotationMatrix, euler.x, glm::vec3 (1.0f, 0.0f, 0.0f));
//            rotationMatrix = glm::rotate (rotationMatrix, euler.y, glm::vec3 (0.0f, 1.0f, 0.0f));
//            rotationMatrix = glm::rotate (rotationMatrix, euler.z, glm::vec3 (0.0f, 0.0f, 1.0f));
        }
        collider.local.box.matRot = rotationMatrix;
        collider.local.box.matRotInverse = glm::inverse(rotationMatrix);
    }

    void UpdateColliderTransform (Collider& collider, glm::mat4 transform)
    {
        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rot;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::mat3 rotation = decomposeMtx(transform, position, rot, scale);
        //rotation = glm::conjugate(rotation);

        collider.local.box.bounds = glm::vec3(collider.local.size.x * scale.x, collider.local.size.y * scale.y, collider.local.size.z * scale.z);
        collider.local.box.center = position;

        collider.local.box.xMax = collider.local.box.bounds.x + collider.local.box.center.x;
        collider.local.box.xMin = -collider.local.box.bounds.x + collider.local.box.center.x;

        collider.local.box.yMax = collider.local.box.bounds.y + collider.local.box.center.y;
        collider.local.box.yMin = -collider.local.box.bounds.y + collider.local.box.center.y;

        collider.local.box.zMax = collider.local.box.bounds.z + collider.local.box.center.z;
        collider.local.box.zMin = -collider.local.box.bounds.z + collider.local.box.center.z;

        glm::mat4 rotationMatrix = glm::mat4(1.f);
        if (collider.local.type == ColliderType::OBB)
        {
            rotationMatrix = rotationMatrix * glm::mat4(rotation);
//            glm::vec3 euler = glm::eulerAngles(rotation);
//            collider.local.box.rotation = euler;
//            rotationMatrix = glm::rotate (rotationMatrix, euler.x, glm::vec3 (1.0f, 0.0f, 0.0f));
//            rotationMatrix = glm::rotate (rotationMatrix, euler.y, glm::vec3 (0.0f, 1.0f, 0.0f));
//            rotationMatrix = glm::rotate (rotationMatrix, euler.z, glm::vec3 (0.0f, 0.0f, 1.0f));
        }
        else if (collider.local.type == ColliderType::OBB2)
        {
            rotationMatrix = rotationMatrix * glm::mat4(rotation);
            glm::vec3 euler = glm::eulerAngles(rot);
            collider.local.box.rotation.y = euler.y;
//            rotationMatrix = glm::rotate (rotationMatrix, euler.x, glm::vec3 (1.0f, 0.0f, 0.0f));
//            rotationMatrix = glm::rotate (rotationMatrix, euler.y, glm::vec3 (0.0f, 1.0f, 0.0f));
//            rotationMatrix = glm::rotate (rotationMatrix, euler.z, glm::vec3 (0.0f, 0.0f, 1.0f));
        }
        collider.local.box.matRot = rotationMatrix;
        collider.local.box.matRotInverse = glm::inverse(rotationMatrix);
    }

    int FindCollisionIndex(Collider& collider, u16 index, ColliderGroup group)
    {
        for (int i = 0; i < collider.local.collisionsList.size(); i++)
        {
            if (collider.local.collisionsList[i].index == index && collider.local.collisionsList[i].group == group)
                return i;
        }
        return -1;
    }

    void UpdateUICollider(Collider& collider, RECTANGLE::LRectangle& rectangle, int windowWidth, int windowHeight)
    {
        const glm::vec2 extra = glm::vec2 (1.0f, 1.0f);
        collider.local.box.xMin = windowWidth * rectangle.base.anchor.x + rectangle.base.position.x - extra.x;
        collider.local.box.yMin = windowHeight * rectangle.base.anchor.y + rectangle.base.position.y - extra.y;
        collider.local.box.xMax = collider.local.box.xMin + rectangle.base.size.x;
        collider.local.box.yMax = collider.local.box.yMin + rectangle.base.size.y;
    }
}