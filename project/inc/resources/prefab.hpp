#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

namespace RESOURCES::PREFAB {

    const char* NODE = "prefab";

    const char* PREFAB_RESERVED    = "\0";
    const char* PREFAB_PLAYER     = "PLAYER";
    const char* PREFAB_INVALID    = "INVALID";

    const u8 PREFABS_COUNT = 3;

    const char* NAME_PREFABS[PREFABS_COUNT] {
        PREFAB_RESERVED,    // 0 is reserved - it means not a prefab!
        PREFAB_PLAYER,
        PREFAB_INVALID,     // last is for error checking !
    };

    enum IDS: u8 {
        PLAYER      = 0,
        INVALID     = 1,
    };

}
