//
// Created by Bartek on 19.04.2024.
//


#ifndef EVILENGINE_MAPGENERATOR_HPP
#define EVILENGINE_MAPGENERATOR_HPP

#endif //EVILENGINE_MAPGENERATOR_HPP

#include "effolkronium/random.hpp"
using Random = effolkronium::random_static;

#include <filesystem>
#include "module.hpp"
#include <algorithm>
#include <cmath>
//#include "../resources/json.hpp"

namespace MAP_GENERATOR {

    struct ParkourDifficulty { // Variables determining which modules should be used for generation
        float rangePosition = 0.25; // Center position of the range used to generate level. 0.5f - median module, 0.f - first module, 1.f - last module.
        float rangeWidth = 0.5f; // Width of the range used to generate level. 1.f = count of loaded modules, 0.5f = half of loaded modules.
    };

    struct Modifiers {
        int levelLength = 5; // Number of modules to create one level
        int stationaryTrapsAmount = 2; // Amount of traps generated on one module.
        int pushingTrapsAmount = 5; // Amount of traps generated on one module.
        ParkourDifficulty parkourDifficulty;
    };

    struct MapGenerator {
        Modifiers modifiers;
        std::vector<MODULE::Module> _loadedModules;
        std::vector<MODULE::Module> _generatedLevel;
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

    MODULE::ModuleType CalculateModuleType(float entranceSide, float exitSide)
    {
        float result = fabs(entranceSide - exitSide);
        switch (int(result))
        {
            case 0:
            case 90:
            case 270:
                return MODULE::ModuleType::WINDING_MODULE;
            case 180:
                return MODULE::ModuleType::STRAIGHT_MODULE;
            default:
                return MODULE::ModuleType::WINDING_MODULE;
        }
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
////            RESOURCES::Json json;
////            std::ifstream file;
////            file.open ( p.path().filename() );
////            file >> json; // Parse the file.
////            json.contains("ROOT");
////            std::string s = json.dump(0);
//
//            MODULE::ModuleType moduleType = CalculateModuleType(loadedEntranceSide, loadedExitSide);
//
//            generator->_loadedModules.emplace_back(MODULE::Module(loadedHeight, loadedEntranceSide, loadedExitSide, loadedParkourDifficulty, moduleType));
//
//        }


        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 90, 1.f, MODULE::ModuleType::WINDING_MODULE,"001"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 270, 0, 3.f, MODULE::ModuleType::WINDING_MODULE,"002"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 90, 90, 5.f, MODULE::ModuleType::WINDING_MODULE,"003"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 180, 270, 4.f, MODULE::ModuleType::WINDING_MODULE,"004"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 90, 0.f, 5.f, MODULE::ModuleType::WINDING_MODULE,"005"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 180, 270, 9.f, MODULE::ModuleType::WINDING_MODULE,"006"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 90, 8.f, MODULE::ModuleType::WINDING_MODULE,"007"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 270, 180, 10.f, MODULE::ModuleType::WINDING_MODULE,"008"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 90, 3.f, MODULE::ModuleType::WINDING_MODULE,"009"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 270, 90, 5.f, MODULE::ModuleType::STRAIGHT_MODULE,"010"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 90, 270, 1.f, MODULE::ModuleType::STRAIGHT_MODULE,"011"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 180.f, 7.f, MODULE::ModuleType::STRAIGHT_MODULE,"012"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 90, 5.f, MODULE::ModuleType::WINDING_MODULE,"013"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 90, 90, 1.f, MODULE::ModuleType::WINDING_MODULE,"014"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 90, 180, 8.f, MODULE::ModuleType::WINDING_MODULE,"015"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 90, 5.f, MODULE::ModuleType::WINDING_MODULE,"016"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 180, 90, 9.f, MODULE::ModuleType::WINDING_MODULE,"017"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 270, 180, 5.f, MODULE::ModuleType::WINDING_MODULE,"020"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 270, 90, 6.f, MODULE::ModuleType::STRAIGHT_MODULE,"021"));
        generator->_loadedModules.emplace_back(MODULE::Module(10, 0.f, 180.f, 2.f, MODULE::ModuleType::STRAIGHT_MODULE,"022"));

        // Sort modules by parkour difficulty
        SortModules(generator);
    }

    void GenerateLevel (MAP_GENERATOR::MG generator)
    {
        float lastExitSide = 0.f;
        std::vector<std::string> loadedModules;

        int rangePosition = round((generator->_loadedModules.size()-1)*generator->modifiers.parkourDifficulty.rangePosition);
        int rangeMin = rangePosition - round(generator->_loadedModules.size()*generator->modifiers.parkourDifficulty.rangeWidth/2);
        rangeMin = std::max(rangeMin, 0);
        int rangeMax = rangePosition + round((generator->_loadedModules.size())*generator->modifiers.parkourDifficulty.rangeWidth/2);
        rangeMax = std::min(rangeMax, int(generator->_loadedModules.size()-1));

        if (rangeMax - rangeMin + 1 < generator->modifiers.levelLength)
        {
            DEBUG { spdlog::info ("Not enough modules in the selected range to generate a level");}
            return;
        }
        else
        {
            for (int i = 0; i < generator->modifiers.levelLength; i++)
            {
                int index = Random::get(rangeMin, rangeMax);
                MODULE::Module module = generator->_loadedModules[index];
                while (count(loadedModules.begin(), loadedModules.end(), module.filepath) != 0)
                {
                    index = Random::get(rangeMin, rangeMax);
                    module = generator->_loadedModules[index];
                }

                DEBUG { spdlog::info("Pushing traps");}
//                RandomIterator iterator(generator->modifiers.pushingTrapsAmount, 0, module.pushableTrapSpotsCount);
//                while(iterator.has_next())
//                {
//                    DEBUG { spdlog::info("{0}", iterator.next());}
//                }
//
//                RandomIterator iterator2(generator->modifiers.stationaryTrapsAmount, 0, module.platformsCount);
//                while(iterator2.has_next())
//                {
//                    DEBUG { spdlog::info("Stationary traps");}
//                    DEBUG { spdlog::info("{0}", iterator2.next());}
//                }

//                for (int j = 0; j < generator->modifiers.pushingTrapsAmount; j++)
//                {
//
//                }
//
//                for (int j = 0; j < generator->modifiers.stationaryTrapsAmount; j++)
//                {
//
//                }

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

            // TODO: combine chosen modules into one scene

            DEBUG { spdlog::info("Generated level: ");}

            for (int i = 0; i < generator->modifiers.levelLength; i++)
            {
                DEBUG { spdlog::info("Module {0}: {1}; Parkour difficulty: {2}; Entrance side: {3}; Exit side: {4}", i, generator->_generatedLevel[i].filepath, generator->_generatedLevel[i].parkourDifficulty, generator->_generatedLevel[i].entranceSide, generator->_generatedLevel[i].exitSide);}
            }

        }
    }
}