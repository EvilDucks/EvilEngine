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
        float rangePosition = 0.5f; // Center position of the range used to generate level. 0.5f - median module, 0.f - first module, 1.f - last module.
        float rangeWidth = 1.0f; // Width of the range used to generate level. 1.f = count of loaded modules, 0.5f = half of loaded modules.
    };

    struct Modifiers {
        int levelLength = 5; // Number of modules to create one level
        int stationaryTrapsAmount = 2; // Amount of traps generated on one module.
        int pushingTrapsAmount = 5; // Amount of traps generated on one module.
        ParkourDifficulty parkourDifficulty;
        float diagonalModuleProbability = 0.75f; // Probability of choosing a diagonal module for generation, probability of choosing a flat module for generation = 1 - diagonalModuleProbability
        float sideBranchProbabilityStep = 0.2f; // Value to increment probability of generating a side branch
    };

    struct MapGenerator {
        Modifiers modifiers;
        std::vector<MODULE::Module> _loadedDiagonalModules;
        std::vector<MODULE::Module> _loadedFlatModules;
        std::vector<std::vector<MODULE::Module>> _generatedLevel;
    };
    using MG = MapGenerator*;

    bool CompareParkourDifficulty(MODULE::Module a, MODULE::Module b)
    {
        return a.parkourDifficulty < b.parkourDifficulty;
    }

    void SortModules(MAP_GENERATOR::MG generator)
    {
        std::sort(generator->_loadedDiagonalModules.begin(), generator->_loadedDiagonalModules.end(), CompareParkourDifficulty);
        std::sort(generator->_loadedFlatModules.begin(), generator->_loadedFlatModules.end(), CompareParkourDifficulty);
    }

    MODULE::ModuleType CalculateModuleType(int entranceSide, int exitSide)
    {
        int result = abs(entranceSide - exitSide);
        switch (result)
        {
            case 0:
            case 1:
            case 3:
                return MODULE::ModuleType::DIAGONAL_MODULE;
            case 2:
                return MODULE::ModuleType::FLAT_MODULE;
            default:
                return MODULE::ModuleType::DIAGONAL_MODULE;
        }
    }

    int ModuleTypeToInt(MODULE::ModuleType type)
    {
        switch(type)
        {
            case MODULE::ModuleType::DIAGONAL_MODULE:
                return 0;
            case MODULE::ModuleType::FLAT_MODULE:
                return 2;
        }
    }

    void LoadModules (MAP_GENERATOR::MG& generator, const char* path)
    {
        for (auto& p : std::filesystem::directory_iterator(path))
        {
            int loadedEntranceSide = 0.f;
            int loadedExitSide = 0.f;
            float loadedParkourDifficulty = 0.f;

            // TODO: Load module from file
            std::string fileStr = p.path().generic_string();
            std::string fileName = p.path().filename().generic_string();

            std::stringstream ss1(fileName);

            getline(ss1, fileName, '.');

            std::vector<std::string> fileNameSplit;
            std::string word;

            std::stringstream ss(fileName);

            while (!ss.eof()) {
                getline(ss, word, '_');
                fileNameSplit.push_back(word);
            }

            if (fileNameSplit[1] != "n")
            {
                loadedParkourDifficulty = std::stof(fileNameSplit[1]);
                loadedExitSide = std::stoi(fileNameSplit[2]);

                std::ifstream f(fileStr);
                RESOURCES::Json json = RESOURCES::Json::parse(f);


                MODULE::ModuleType moduleType = CalculateModuleType(loadedEntranceSide, loadedExitSide);

                if (moduleType == MODULE::ModuleType::DIAGONAL_MODULE)
                {
                    generator->_loadedDiagonalModules.emplace_back(MODULE::Module(0, 90, loadedParkourDifficulty, moduleType, fileName, 0, 0, json));
                }
                else
                {
                    generator->_loadedFlatModules.emplace_back(MODULE::Module(0, 90, loadedParkourDifficulty, moduleType, fileName, 0, 0, json));
                }

            }
        }

        // Sort modules by parkour difficulty
        SortModules(generator);
    }

    void FindConnection (MAP_GENERATOR::MG generator, int& i, int& branchHeight, int mainBranchHeight, int rangeMinDiagonal, int rangeMaxDiagonal, int rangeMinFlat, int rangeMaxFlat)
    {
        if (branchHeight == mainBranchHeight)
        {
            return;
        }
        int dif = generator->_generatedLevel[1][i].moduleHeight - generator->_generatedLevel[0][i+1].moduleHeight;
        if (generator->_generatedLevel[1][i].type == MODULE::ModuleType::DIAGONAL_MODULE)
        {
            dif++;
        }

        MODULE::Module module;
        int index;
        switch(dif)
        {
            case -1:
                index = Random::get(rangeMinDiagonal, rangeMaxDiagonal);
                module = generator->_loadedDiagonalModules[index];
                module.moduleHeight = branchHeight;
                module.direction = MODULE::ModuleDirection::CW;
                module.rotation = (generator->_generatedLevel[1][i].rotation + 270) % 360;
                branchHeight++;
                generator->_generatedLevel[1].emplace_back(module);
                if (branchHeight == mainBranchHeight)
                {
                    return;
                }
                if (branchHeight == generator->_generatedLevel[0][i+2].moduleHeight && module.rotation == generator->_generatedLevel[0][i+2].rotation)
                {
                    i++;
                    return;
                }
                else
                {
                    index = Random::get(rangeMinFlat, rangeMaxFlat);
                    module = generator->_loadedFlatModules[index];
                    module.moduleHeight = branchHeight;
                    module.direction = MODULE::ModuleDirection::CW;
                    module.rotation = (generator->_generatedLevel[1][i+1].rotation + 270) % 360;
                    i+=2;
                    return;
                }
//            case 1:
//            case 0:
            default:
                break;
        }
    }

    void GenerateLevel (MAP_GENERATOR::MG generator)
    {
        std::vector<MODULE::Module> mainBranch;
        std::vector<MODULE::Module> sideBranch;
        generator->_generatedLevel.emplace_back(mainBranch);
        generator->_generatedLevel.emplace_back(sideBranch);
        int lastExitSide = 0;
        std::vector<std::string> loadedModules;

        int rangePositionDiagonal = round((generator->_loadedDiagonalModules.size()-1)*generator->modifiers.parkourDifficulty.rangePosition);
        int rangeMinDiagonal = rangePositionDiagonal - round(generator->_loadedDiagonalModules.size()*generator->modifiers.parkourDifficulty.rangeWidth/2);
        rangeMinDiagonal = std::max(rangeMinDiagonal, 0);
        int rangeMaxDiagonal = rangePositionDiagonal + round((generator->_loadedDiagonalModules.size())*generator->modifiers.parkourDifficulty.rangeWidth/2);
        rangeMaxDiagonal = std::min(rangeMaxDiagonal, int(generator->_loadedDiagonalModules.size()-1));

        int rangePositionFlat = round((generator->_loadedFlatModules.size()-1)*generator->modifiers.parkourDifficulty.rangePosition);
        int rangeMinFlat = rangePositionFlat - round(generator->_loadedFlatModules.size()*generator->modifiers.parkourDifficulty.rangeWidth/2);
        rangeMinFlat = std::max(rangeMinFlat, 0);
        int rangeMaxFlat = rangePositionFlat + round((generator->_loadedFlatModules.size())*generator->modifiers.parkourDifficulty.rangeWidth/2);
        rangeMaxFlat = std::min(rangeMaxFlat, int(generator->_loadedFlatModules.size()-1));

        int branchHeight = 0;
        MODULE::Module lastModule;
        lastModule.type = MODULE::ModuleType::FLAT_MODULE;
        lastModule.rotation = 270;
        if (rangeMaxDiagonal - rangeMinDiagonal + 1 < generator->modifiers.levelLength || rangeMaxFlat - rangeMinFlat + 1 < generator->modifiers.levelLength)
        {
            DEBUG { spdlog::info ("Not enough modules in the selected range to generate a level");}
            return;
        }
        else
        {
            while (branchHeight < generator->modifiers.levelLength)
            {
                // Choosing based on selected probability whether this module will be diagonal or flat
                bool diagonalModule = Random::get<bool>(generator->modifiers.diagonalModuleProbability);
                MODULE::Module module;


                // Choosing which module to use for generation
                if (!diagonalModule && lastModule.type != MODULE::ModuleType::FLAT_MODULE)
                {
                    int index = Random::get(rangeMinFlat, rangeMaxFlat);
                    module = generator->_loadedFlatModules[index];
                    while (count(loadedModules.begin(), loadedModules.end(), module.fileName) != 0)
                    {
                        index = Random::get(rangeMinFlat, rangeMaxFlat);
                        module = generator->_loadedFlatModules[index];
                    }
                    module.moduleHeight = branchHeight;
                }
                else
                {
                    int index = Random::get(rangeMinDiagonal, rangeMaxDiagonal);
                    module = generator->_loadedDiagonalModules[index];
                    while (count(loadedModules.begin(), loadedModules.end(), module.fileName) != 0)
                    {
                        index = Random::get(rangeMinDiagonal, rangeMaxDiagonal);
                        module = generator->_loadedDiagonalModules[index];
                    }
                    module.moduleHeight = branchHeight;
                    branchHeight++;
                }


//                DEBUG { spdlog::info("Pushing traps");}
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
                if (module.direction != MODULE::ModuleDirection::CW && lastModule.direction != MODULE::ModuleDirection::CW)
                {
                    module.rotation = (lastModule.rotation+90)%360;
                }
                else if (module.direction == MODULE::ModuleDirection::CW && lastModule.direction == MODULE::ModuleDirection::CW)
                {
                    module.rotation = (lastModule.rotation+270)%360;
                }

                // TODO: move root in y-axis
                generator->_generatedLevel[0].emplace_back(module);
                loadedModules.emplace_back(module.fileName);
                lastModule = module;
            }

            int mainBranchHeight = branchHeight;

            branchHeight = 0;
            //int sideLength = 0;
            float branchProbability = 0.f;

            for (int i = 0; branchHeight < generator->modifiers.levelLength; i++)
            {
                // Choosing based on selected probability whether this module will be diagonal or flat
                bool diagonalModule = Random::get<bool>(generator->modifiers.diagonalModuleProbability);
                bool generateSideBranch = Random::get<bool>(branchProbability);
                MODULE::Module module;

                if (generateSideBranch)
                {
                    module.type = MODULE::ModuleType::NONE;
                    branchHeight++;
                    branchProbability+=generator->modifiers.sideBranchProbabilityStep;
                }
                else
                {
                    // Choosing which module to use for generation
                    if (!diagonalModule && lastModule.type != MODULE::ModuleType::FLAT_MODULE) {
                        int index = Random::get(rangeMinFlat, rangeMaxFlat);
                        module = generator->_loadedFlatModules[index];
//                        while (count(loadedModules.begin(), loadedModules.end(), module.fileName) != 0) {
//                            index = Random::get(rangeMinFlat, rangeMaxFlat);
//                            module = generator->_loadedFlatModules[index];
//                        }
                        module.moduleHeight = branchHeight;
                    }
                    else {
                        int index = Random::get(rangeMinDiagonal, rangeMaxDiagonal);
                        module = generator->_loadedDiagonalModules[index];
//                        while (count(loadedModules.begin(), loadedModules.end(), module.fileName) != 0) {
//                            index = Random::get(rangeMinDiagonal, rangeMaxDiagonal);
//                            module = generator->_loadedDiagonalModules[index];
//                        }
                        module.moduleHeight = branchHeight;
                        branchHeight++;
                    }

                    module.direction = MODULE::ModuleDirection::CW;

                    module.rotation = (generator->_generatedLevel[0][i].rotation + 270) % 360;
                }

                generator->_generatedLevel[1].emplace_back(module);
                if (generateSideBranch)
                {
                    FindConnection(generator, i, branchHeight, mainBranchHeight, rangeMinDiagonal, rangeMaxDiagonal, rangeMinFlat, rangeMaxFlat);
                }
                i++;
            }

            DEBUG { spdlog::info("Generated level: ");}

            for (int i = 0; i < generator->modifiers.levelLength; i++)
            {
                DEBUG { spdlog::info("Module {0}: {1}; Parkour difficulty: {2}; Module type: {3}; Entrance: {4}; Exit: {5}", i, generator->_generatedLevel[0][i].fileName, generator->_generatedLevel[0][i].parkourDifficulty, MODULE::ModuleTypeToString(generator->_generatedLevel[0][i].type), generator->_generatedLevel[0][i].entranceSide, generator->_generatedLevel[0][i].exitSide);}
            }

        }
    }
}