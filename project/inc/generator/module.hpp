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
        int moduleHeight = 0; // Number of "layers" in y-axis, assuming one layer means one block/or some established height
        int entranceSide = 0; // Which side of the tower is the entrance (North = 0, East = 90, South = 180, West = 270)
        int exitSide = 0; // Which side of the tower is the exit (North = 0, East = 90, South = 180, West = 270)
        float parkourDifficulty = 5.f; // from 0.f - easiest to 10.f - hardest
        ModuleType type; // Module type, either WINDING_MODULE (abs(entranceSide-exitSide) > 180) or STRAIGHT_MODULE (abs(entranceSide-exitSide) = 180), assuming we do not have levels where (abs(entranceSide-exitSide) < 180)
        std::string fileName; // Temporary parameter to be deleted, not defined by module json
        int platformsCount = 20; // Number of traversible platforms that can have stationary traps
        int pushableTrapSpotsCount = 10; // Number of possible pushable traps spots
        RESOURCES::Json json; // Json of the module
    };

    std::string ModuleTypeToString(ModuleType type)
    {
        switch(type)
        {
            case ModuleType::WINDING_MODULE:
                return "WINDING";
            case ModuleType::STRAIGHT_MODULE:
            default:
                return "STRAIGHT";
        }
    };


}