#pragma once
#include "tool/debug.hpp"

namespace PARENTHOOD {

    struct Base {
        u16 childrenCount = 0;
        GameObjectID* children = nullptr;
    };

    // ChildrenCollection = 2
    // size, parent( size, child, child, child), parent(size, child, child)

    struct Parenthood {
        GameObjectID id = 0;
        Base base { 0 };
    };

}