//
// Created by Bartek on 08.04.2024.
//

#ifndef EVILENGINE_PLAYER_HPP
#define EVILENGINE_PLAYER_HPP

#endif //EVILENGINE_PLAYER_HPP

#include <iostream>
#include "tool/debug.hpp"
#include "render/mesh.hpp"
#include "hid/inputManager.hpp"


namespace PLAYER {


    struct Base {
        std::vector<InputDevice> controlScheme;
        std::string name;
    };

    struct Player {
        GameObjectID id = 0;

        Base local;
    };


}