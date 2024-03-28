#pragma once
#include "tool/debug.hpp"

namespace PARENTHOOD {

    struct Base {
        u64 childrenCount = 0;
        GameObjectID* children = nullptr;
    };

    struct Parenthood {
        GameObjectID id = 0;
        Base base { 0 };
    };

}