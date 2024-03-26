#pragma once
#include "tool/debug.hpp"

namespace PARENTSHIP {

    struct Base {
        GameObjectID parent = 0;
        u64 childrenCount = 0;
        GameObjectID* children = nullptr;
    };

    struct Parentship {
        GameObjectID id = 0;
        Base base = {0};
    };

}

// World (root) 
//  -> Child1
//      -> Child1
//      -> Child2
//      -> Child3
//  -> Child2
//      -> Child1
//      -> Child2
//      -> Child3
//  -> Child3
//      -> Child1
//      -> Child2
//      -> Child3
//
// In such graph there's 4 parentships
// Wolrd - 3children is 1 parentship
// Child1 - 3children is 1 parentship
// Child2 - 3children is 1 parentship
// Child3 - 3children is 1 parentship