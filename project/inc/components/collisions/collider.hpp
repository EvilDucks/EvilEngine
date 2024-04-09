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

    struct Base {
        Scale size = glm::vec3(1.f);
        IsTrigger isTrigger;
        ColliderType type;
        ColliderGroup group;
        bool isEnabled = true;
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

        // if object with colliders has transform we rescale its size
        if (transformIndex)
        {
            collider.local.size = glm::vec3(collider.local.size.x * transforms[transformIndex].local.scale.x, collider.local.size.y * transforms[transformIndex].local.scale.y, collider.local.size.z * transforms[transformIndex].local.scale.z);
        }
    }
}