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

namespace COLLIDER {
    using Scale = glm::vec3;
    using IsTrigger = bool;

    enum class ColliderType {
        AABB,
        SPHERE,
        OBB,
        PLANE
    };

    enum class ColliderGroup {
        PLAYER,
        MAP,
        HAZARDS,
        UI
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
    };

    struct Collision {
        u16 id;
        ColliderGroup group;
        glm::vec3 overlap = glm::vec3(0.f);
    };

    struct Base {
        Scale size = glm::vec3(1.f);
        IsTrigger isTrigger;
        ColliderType type;
        ColliderGroup group;
        bool isEnabled = true;
        //TRANSFORM::Transform* transform = nullptr;
        BoundingBox box;
        std::vector<Collision> collisionsList;
    };

    struct Collider {
        GameObjectID id = 0;
        Base local;
    };

    void InitializeColliderSize(Collider& collider, MESH::Mesh& mesh, int transformsCount, TRANSFORM::LTransform* transforms) {
        PROFILER { ZoneScopedN("Collider: InitializeColliderSize"); }

        // assuming meshes are in interval from -x to x
        collider.local.size = glm::vec3((abs(mesh.base.boundsMin.x) + abs(mesh.base.boundsMax.x))/2.f, (abs(mesh.base.boundsMin.y) + abs(mesh.base.boundsMax.y))/2.f, (abs(mesh.base.boundsMin.z) + abs(mesh.base.boundsMax.z))/2.f);
        u64 transformIndex = OBJECT::ID_DEFAULT;
        OBJECT::GetComponentSlow<TRANSFORM::LTransform>(transformIndex, transformsCount, transforms, collider.id);

        collider.local.box.bounds = glm::vec3(collider.local.size.x * transforms[transformIndex].base.scale.x, collider.local.size.y * transforms[transformIndex].base.scale.y, collider.local.size.z * transforms[transformIndex].base.scale.z);
        collider.local.box.center = transforms[transformIndex].base.position;

        collider.local.box.xMax = collider.local.box.bounds.x + collider.local.box.center.x;
        collider.local.box.xMin = -collider.local.box.bounds.x + collider.local.box.center.x;

        collider.local.box.yMax = collider.local.box.bounds.y + collider.local.box.center.y;
        collider.local.box.yMin = -collider.local.box.bounds.y * + collider.local.box.center.y;

        collider.local.box.zMax = collider.local.box.bounds.z + collider.local.box.center.z;
        collider.local.box.zMin = -collider.local.box.bounds.z + collider.local.box.center.z;

        //if (collider.local.type == ColliderType::OBB)
        {
            glm::mat4 rotationMatrix = glm::rotate (glm::mat4(1.f), glm::radians (transforms[transformIndex].base.rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
            rotationMatrix = glm::rotate (rotationMatrix, glm::radians (transforms[transformIndex].base.rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
            rotationMatrix = glm::rotate (rotationMatrix, glm::radians (transforms[transformIndex].base.rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
            collider.local.box.matRot = rotationMatrix;
            collider.local.box.matRot = glm::inverse(rotationMatrix);
        }
        // if object with colliders has transform we rescale its size
//        if (transformIndex)
//        {
//            collider.local.size = glm::vec3(collider.local.size.x * transforms[transformIndex].local.scale.x, collider.local.size.y * transforms[transformIndex].local.scale.y, collider.local.size.z * transforms[transformIndex].local.scale.z);
//        }
    }

    void UpdateColliderTransform (Collider& collider, TRANSFORM::LTransform& transform)
    {
        collider.local.box.bounds = glm::vec3(collider.local.size.x * transform.base.scale.x, collider.local.size.y * transform.base.scale.y, collider.local.size.z * transform.base.scale.z);
        collider.local.box.center = transform.base.position;

        collider.local.box.xMax = collider.local.box.bounds.x + collider.local.box.center.x;
        collider.local.box.xMin = -collider.local.box.bounds.x + collider.local.box.center.x;

        collider.local.box.yMax = collider.local.box.bounds.y + collider.local.box.center.y;
        collider.local.box.yMin = -collider.local.box.bounds.y * + collider.local.box.center.y;

        collider.local.box.zMax = collider.local.box.bounds.z + collider.local.box.center.z;
        collider.local.box.zMin = -collider.local.box.bounds.z + collider.local.box.center.z;

        //if (collider.local.type == ColliderType::OBB)
        {
            glm::mat4 rotationMatrix = glm::rotate (glm::mat4(1.f), glm::radians (transform.base.rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
            rotationMatrix = glm::rotate (rotationMatrix, glm::radians (transform.base.rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
            rotationMatrix = glm::rotate (rotationMatrix, glm::radians (transform.base.rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
            collider.local.box.matRot = rotationMatrix;
            collider.local.box.matRot = glm::inverse(rotationMatrix);
        }

    }

    int FindCollisionIndexById(Collider& collider, GameObjectID id)
    {
        for (int i = 0; i < collider.local.collisionsList.size(); i++)
        {
            if (collider.local.collisionsList[i].id == id)
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