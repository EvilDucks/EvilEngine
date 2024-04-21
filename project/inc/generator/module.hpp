//
// Created by Bartek on 19.04.2024.
//

#ifndef EVILENGINE_MODULE_HPP
#define EVILENGINE_MODULE_HPP

#endif //EVILENGINE_MODULE_HPP

#include <iostream>
#include <vector>
#include "types.hpp"

namespace MODULE {
//    struct ModuleRatings {
//        int moduleHeight = 0;
//        int movablePlatformsCount = 0;
//        int trapSpotsCount = 0;
//        //float difficultyLevel = 0.f; // Amount of traps generated on level. 0 - no traps; 1 - maximum amount of traps
//        //float complexityLevel = 0.f;
//        //float freqOfDynamicObject = 0.f;
//        //float shakeOfDynamicObjects = 0.f;
//    };

    struct Module {
        //u16 id = 0;
        //ModuleRatings ratings;

        int moduleHeight = 0; // Number of "layers" in y-axis
        int movablePlatformsCount = 0;
        int trapSpotsCount = 0;
        float entranceSide = 0.f; // Which side of the tower is the entrance/exit (North = 0.f, East = 90.f, South = 180.f, West = 270.f)
        float exitSide = 0.f;

        //float difficultyLevel = 0.f; // Amount of traps generated on level. 0 - no traps; 1 - maximum amount of traps
        //float complexityLevel = 0.f;
        //float freqOfDynamicObject = 0.f;
        //float shakeOfDynamicObjects = 0.f;
    };


}