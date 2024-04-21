//
// Created by Bartek on 19.04.2024.
//


#ifndef EVILENGINE_MAPGENERATOR_HPP
#define EVILENGINE_MAPGENERATOR_HPP

#endif //EVILENGINE_MAPGENERATOR_HPP


#include <filesystem>
#include "module.hpp"

namespace MAP_GENERATOR {

    struct Modifiers {
        int levelLength = 5; // Number of modules to create one level
        float difficultyLevel = 0.5; // Amount of traps generated on level. 0 - no traps; 1 - maximum amount of traps
        float complexityLevel = 0.5;
        float freqOfDynamicObject = 0.5;
        float shakeOfDynamicObjects = 0.5;
    };

    struct Base {

    };

    struct MapGenerator {
        Modifiers modifiers;
        std::vector<MODULE::Module> _loadedModules;
        std::vector<MODULE::Module> _generatedLevel;
        Base local;
    };

    void LoadModules (MapGenerator generator, std::filesystem::path path)
    {
        for (auto& p : std::filesystem::directory_iterator(path))
        {
            int loadedHeight = 0;
            int loadedMovablePlatformsCount = 0;
            int loadedTrapSpotsCount = 0;

            // TODO: Load module from file


            generator._loadedModules.emplace_back(MODULE::Module(loadedHeight, loadedMovablePlatformsCount, loadedTrapSpotsCount));

        }
    }

    void GenerateLevel (MapGenerator generator)
    {
        float lastExitSide = 0.f;
        for (int i = 0; i < generator.modifiers.levelLength; i++)
        {
            // TODO: Choose module from loadedModules
            MODULE::Module module;



            while (module.entranceSide != lastExitSide)
            {
                // TODO: rotate module root and update entrance and exit side
            }

            lastExitSide = module.exitSide;
        }
    }

}