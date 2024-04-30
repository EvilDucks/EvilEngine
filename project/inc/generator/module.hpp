//
// Created by Bartek on 19.04.2024.
//

#ifndef EVILENGINE_MODULE_HPP
#define EVILENGINE_MODULE_HPP

#endif //EVILENGINE_MODULE_HPP

#include "types.hpp"
#include "randomIterator.hpp"

namespace MODULE {

    enum class ModuleType {
        WINDING_MODULE,
        STRAIGHT_MODULE
    };

    struct Module {
        //u16 id = 0;
        //ModuleRatings ratings;
        //int trapSpotsCount = 0;

        int moduleHeight = 0; // Number of "layers" in y-axis
        int entranceSide = 0; // Which side of the tower is the entrance/exit (North = 0.f, East = 90.f, South = 180.f, West = 270.f)
        int exitSide = 0;
        float parkourDifficulty = 5.f; // from 0.f - easiest to 10.f - hardest
        ModuleType type;
        std::string filepath;
        int platformsCount = 20;
        int pushableTrapSpotsCount = 10;
        RESOURCES::Json json;
    };


}