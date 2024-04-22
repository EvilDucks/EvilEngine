//
// Created by Bartek on 19.04.2024.
//


#ifndef EVILENGINE_MAPGENERATOR_HPP
#define EVILENGINE_MAPGENERATOR_HPP

#endif //EVILENGINE_MAPGENERATOR_HPP


#include <filesystem>
#include "module.hpp"
#include <algorithm>
#include <cmath>


namespace MAP_GENERATOR {

    struct ParkourDifficulty {
        float rangePosition = 0.4;
        float rangeWidth = 0.5f;
    };

    struct Modifiers {
        int levelLength = 5; // Number of modules to create one level
        float stationaryTrapsAmount = 0.5; // Amount of traps generated on level. 0 - no traps; 1 - maximum amount of traps
        float pushingTrapsAmount = 0.5; // Amount of traps generated on level. 0 - no traps; 1 - maximum amount of traps
        ParkourDifficulty parkourDifficulty;
    };

    struct Base {

    };

    struct MapGenerator {
        Modifiers modifiers;
        std::vector<MODULE::Module> _loadedModules;
        std::vector<MODULE::Module> _generatedLevel;
        Base local;
    };
    using MG = MapGenerator*;

    bool CompareParkourDifficulty(MODULE::Module a, MODULE::Module b)
    {
        return a.parkourDifficulty < b.parkourDifficulty;
    }

    void SortModules(MAP_GENERATOR::MG generator)
    {
        std::sort(generator->_loadedModules.begin(), generator->_loadedModules.end(), CompareParkourDifficulty);
    }



    void LoadModules (MAP_GENERATOR::MG generator, std::filesystem::path path)
    {
//        for (auto& p : std::filesystem::directory_iterator(path))
//        {
//            int loadedHeight = 0;
//            float loadedEntranceSide = 0.f;
//            float loadedExitSide = 0.f;
//            float loadedParkourDifficulty = 0.f;
//
//            // TODO: Load module from file
//
//
//            generator->_loadedModules.emplace_back(MODULE::Module(loadedHeight, loadedEntranceSide, loadedExitSide, loadedParkourDifficulty));
//
//        }

        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 90, 1.f, "001"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 270, 0, 3.f, "002"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 90, 90, 5.f, "003"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 180, 270, 4.f, "004"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 90, 0.f, 5.f, "005"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 180, 270, 9.f, "006"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 90, 8.f, "007"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 270, 180, 10.f, "008"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 90, 3.f, "009"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 270, 90, 5.f, "010"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 90, 90, 1.f, "011"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 0.f, 7.f, "012"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 90, 5.f, "013"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 90, 90, 1.f, "014"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 90, 180, 8.f, "015"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 90, 5.f, "016"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 180, 90, 9.f, "017"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 270, 180, 5.f, "020"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 270, 90, 6.f, "021"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 0.f, 2.f, "022"));

        // Sort modules by parkour difficulty
        SortModules(generator);
    }

    void GenerateLevel (MAP_GENERATOR::MG generator)
    {
        srand (time(NULL));
        float lastExitSide = 0.f;
        std::vector<std::string> loadedModules;

        int rangePosition = round((generator->_loadedModules.size()-1)*generator->modifiers.parkourDifficulty.rangePosition);
        int rangeMin = rangePosition - round(generator->_loadedModules.size()*generator->modifiers.parkourDifficulty.rangeWidth/2);
        rangeMin = std::max(rangeMin, 0);
        int rangeMax = rangePosition + round((generator->_loadedModules.size())*generator->modifiers.parkourDifficulty.rangeWidth/2);
        rangeMax = std::min(rangeMax, int(generator->_loadedModules.size()-1));

        for (int i = 0; i < generator->modifiers.levelLength; i++)
        {
            MODULE::Module module = generator->_loadedModules[rand() % rangeMax + rangeMin];
            while (count(loadedModules.begin(), loadedModules.end(), module.filepath) != 0)
            {
                srand (time(NULL));
                // TODO: check if level generation is possible
                module = generator->_loadedModules[rand() % rangeMax + rangeMin];
            }

            while (module.entranceSide != lastExitSide)
            {
                // TODO: rotate and move in y-axis module's root
                module.entranceSide += 90;
                module.entranceSide %= 360;
                module.exitSide += 90;
                module.exitSide %= 360;
            }

            // TODO: move root in y-axis
            generator->_generatedLevel.emplace_back(module);
            loadedModules.emplace_back(module.filepath);
            lastExitSide = module.exitSide;
        }
    }

}