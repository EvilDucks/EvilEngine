//
// Created by Bartek on 08.04.2024.
//

#ifndef EVILENGINE_PLAYER_HPP
#define EVILENGINE_PLAYER_HPP

#endif //EVILENGINE_PLAYER_HPP

#include <iostream>
#include "tool/debug.hpp"
#include "render/mesh.hpp"
#include "hid/inputDevices.hpp"

namespace PLAYER {


    struct Base {
        int playerIndex;
        InputDevice device;

    };

    struct Player {
        GameObjectID id = 0;

        Base local;
    };
}