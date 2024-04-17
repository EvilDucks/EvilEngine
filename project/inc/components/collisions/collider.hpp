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


namespace COLLIDER {
    using Scale = glm::vec3;
    using IsTrigger = bool;

    enum class ColliderType {
        AABB,
        SPHERE
    };

    enum class ColliderGroup {
        PLAYER,
        MAP,
        HAZARDS
    };

    struct BoundingBox {
        float xMin = 0;
        float xMax = 0;
        float yMin = 0;
        float yMax = 0;
        float zMin = 0;
        float zMax = 0;
    };

    struct Collision {
        u16 id;
        ColliderGroup group;
        glm::vec3 overlap = glm::vec3(0.f);
    };

    struct Base {
        Scale size = glm::vec3(1.f);
        Scale currentSize = glm::vec3(1.f);
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

    void InitializeColliderSize(Collider& collider, MESH::Mesh& mesh, int transformsCount, TRANSFORM::Transform* transforms) {
        // assuming meshes are in interval from -x to x
        collider.local.size = glm::vec3((abs(mesh.base.boundsMin.x) + abs(mesh.base.boundsMax.x))/2.f, (abs(mesh.base.boundsMin.y) + abs(mesh.base.boundsMax.y))/2.f, (abs(mesh.base.boundsMin.z) + abs(mesh.base.boundsMax.z))/2.f);
        u64 transformIndex = OBJECT::ID_DEFAULT;
        OBJECT::GetComponentSlow<TRANSFORM::Transform>(transformIndex, transformsCount, transforms, collider.id);


        collider.local.box.xMax = (collider.local.size.x ) * transforms[transformIndex].local.scale.x+ transforms[transformIndex].local.position.x;
        collider.local.box.xMin = (-collider.local.size.x ) * transforms[transformIndex].local.scale.x+ transforms[transformIndex].local.position.x;

        collider.local.box.yMax = (collider.local.size.y ) * transforms[transformIndex].local.scale.y+ transforms[transformIndex].local.position.y;
        collider.local.box.yMin = (-collider.local.size.y ) * transforms[transformIndex].local.scale.y+ transforms[transformIndex].local.position.y;

        collider.local.box.zMax = (collider.local.size.z ) * transforms[transformIndex].local.scale.z+ transforms[transformIndex].local.position.z;
        collider.local.box.zMin = (-collider.local.size.z ) * transforms[transformIndex].local.scale.z+ transforms[transformIndex].local.position.z;
        // if object with colliders has transform we rescale its size
//        if (transformIndex)
//        {
//            collider.local.size = glm::vec3(collider.local.size.x * transforms[transformIndex].local.scale.x, collider.local.size.y * transforms[transformIndex].local.scale.y, collider.local.size.z * transforms[transformIndex].local.scale.z);
//        }
    }

    void UpdateColliderTransform (Collider& collider, TRANSFORM::Transform& transform)
    {
        collider.local.box.xMax = (collider.local.size.x ) * transform.local.scale.x+ transform.local.position.x;
        collider.local.box.xMin = (-collider.local.size.x ) * transform.local.scale.x+ transform.local.position.x;

        collider.local.box.yMax = (collider.local.size.y ) * transform.local.scale.y+ transform.local.position.y;
        collider.local.box.yMin = (-collider.local.size.y ) * transform.local.scale.y+ transform.local.position.y;

        collider.local.box.zMax = (collider.local.size.z ) * transform.local.scale.z+ transform.local.position.z;
        collider.local.box.zMin = (-collider.local.size.z ) * transform.local.scale.z+ transform.local.position.z;

    }
}