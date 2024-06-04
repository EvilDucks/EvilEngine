//
// Created by Bartek on 23.05.2024.
//

#ifndef EVILENGINE_COLLISIONMANAGER_HPP
#define EVILENGINE_COLLISIONMANAGER_HPP

#include <iostream>
#include "collider.hpp"
#include "types.hpp"
#include <vector>
#include <functional>

namespace COLLISION::MANAGER {

    using CollisionCallbackFunc = std::function<bool(COLLIDER::Collider, COLLIDER::Collider, glm::vec3)>;

    struct CollisionEvent {
        std::string eventName; // event name
        COLLIDER::Collider collider1;
        COLLIDER::Collider collider2;
        glm::vec3 overlap;
    };

    struct CollisionCallback {
        std::string Ref;
        CollisionCallbackFunc Func;
    };

    struct CollisionManager {
        std::unordered_map<std::string, std::vector<CollisionCallback>> _collisionCallbacks {};
    };
    using CM = CollisionManager*;

    void RegisterCollisionCallback (COLLISION::MANAGER::CM manager, const std::string& elementName, const CollisionCallback& callback);
    void PropagateCollisionEvent (COLLISION::MANAGER::CM manager, CollisionEvent event);
    //int FindUIElementByName(COLLISION::MANAGER::CM manager, UI::ElementType elementType, std::string elementName);

    void RegisterCollisionCallback (COLLISION::MANAGER::CM manager, const std::string& elementName, const CollisionCallback& callback)
    {
        manager->_collisionCallbacks[elementName].emplace_back(callback);
    }

    void PropagateCollisionEvent (COLLISION::MANAGER::CM manager, COLLISION::MANAGER::CollisionEvent event)
    {
        for (size_t i = manager->_collisionCallbacks[event.eventName].size() - 1; i >= 0; i--) {
            auto& collisionCallback = manager->_collisionCallbacks[event.eventName][i];

            if (collisionCallback.Func(event.collider1, event.collider2, event.overlap)) break;
        }
    }

//    int FindUIElementByName(COLLISION::MANAGER::CM manager, UI::ElementType elementType, std::string elementName)
//    {
//        switch (elementType){
//            case UI::ElementType::BUTTON:
//                for (int i = 0; i < manager->buttonsCount; i++)
//                {
//                    if (manager->buttons[i].local.name == elementName)
//                        return i;
//                }
//                break;
//            default:
//                return -1;
//        }
//        return -1;
//    }


    void HandleTriggerCollisions (COLLISION::MANAGER::CM& collisionManager, COLLIDER::Collider& collider, std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount)
    {
        PROFILER { ZoneScopedN("Player: HandlePlayerCollisions"); }

        for (int i = collider.local.collisionsList.size()-1; i >= 0; i--)
        {
            COLLIDER::Collision _collision = collider.local.collisionsList[i];

            switch (_collision.group){
                case COLLIDER::ColliderGroup::PLAYER:
                    COLLISION::MANAGER::PropagateCollisionEvent(collisionManager, COLLISION::MANAGER::CollisionEvent(collider.local.collisionEventName,collider, colliders[_collision.group][_collision.index], _collision.overlap));
                    break;
                default:
                    break;
            }
            auto v = colliders[_collision.group][_collision.index].local.collisionsList;
            //colliders[_collision.group][colliderIndex].local.collisionsList.erase(colliders[_collision.group][colliderIndex].local.collisionsList.begin() + COLLIDER::FindCollisionIndexById(colliders[_collision.group][colliderIndex], collider.id));
            collider.local.collisionsList.erase(collider.local.collisionsList.begin() + i);
        }
    }
}

#endif //EVILENGINE_COLLISIONMANAGER_HPP
