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

    void InitializeColliderSize(Collider& collider, MESH::Mesh* mesh) {
        collider.local.size = glm::vec3((abs(mesh->base.boundsMin.x) + abs(mesh->base.boundsMin.x))/2.f, (abs(mesh->base.boundsMin.y) + abs(mesh->base.boundsMin.y))/2.f, (abs(mesh->base.boundsMin.z) + abs(mesh->base.boundsMin.z))/2.f);
    }
}