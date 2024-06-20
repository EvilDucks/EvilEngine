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
        float stationaryTrapsAmount = 1; // Percentage of traps activated on level (1 - 100%, 0 - 0%).
        int pushingTrapsAmount = 5; // Amount of traps generated on one module.
        int checkpointsSpacing = 2;
        ParkourDifficulty parkourDifficulty;
        float diagonalModuleProbability = 0.60f; // Probability of choosing a diagonal module for generation, probability of choosing a flat module for generation = 1 - diagonalModuleProbability
        float sideBranchProbabilityStep = 0.4f; // Value to increment probability of generating a side branch
    };

    struct MapGenerator {
        Modifiers modifiers;
        std::vector<MODULE::Module> _loadedDiagonalModules;
        std::vector<MODULE::Module> _loadedFlatModules;
        std::vector<MODULE::Module> _loadedCenterModules;
        std::vector<MODULE::Module> _generatedLevelMainBranch;
        std::vector<MODULE::Module> _generatedLevelSideBranch;
        std::vector<MODULE::Module> _generatedLevelCenter;
        std::vector<bool> _generatedSpringTraps;
        std::vector<bool> _generatedCheckpoints;
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

    int ModuleTypeToInt(MODULE::ModuleType type)
    {
        switch(type)
        {
            case MODULE::ModuleType::DIAGONAL_MODULE:
                return 0;
            case MODULE::ModuleType::FLAT_MODULE:
                return 1;
            case MODULE::ModuleType::CENTER:
                return 2;
            default:
                return 3;
        }
    }

    int CalculateSegmentIndex(MAP_GENERATOR::MG generator, u8 difficulty, u8 type)
    {
        switch(type)
        {
            case 0:
                return int(difficulty) - 1;
            case 1:
                return int(generator->_loadedDiagonalModules.size()) + int(difficulty) - 1;
            case 2:
                return int(generator->_loadedDiagonalModules.size()) + int(generator->_loadedFlatModules.size()) + int(difficulty) - 1;
            default:
                break;
        }
        return 0;
    }

    void LoadModules (MAP_GENERATOR::MG& generator, const char* path)
    {
        for (auto& p : std::filesystem::directory_iterator(path))
        {
            int loadedEntranceSide = 0.f;
            int loadedExitSide = 0.f;
            float loadedParkourDifficulty = 0.f;

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

                if (fileNameSplit[2] == "0")
                {
                    generator->_loadedDiagonalModules.emplace_back(MODULE::Module(0, 90, loadedParkourDifficulty, MODULE::ModuleType::DIAGONAL_MODULE, fileName, 0, 0, json));
                }
                else if (fileNameSplit[2] == "1")
                {
                    generator->_loadedFlatModules.emplace_back(MODULE::Module(0, 90, loadedParkourDifficulty, MODULE::ModuleType::FLAT_MODULE, fileName, 0, 0, json));
                }
                else
                {
                    generator->_loadedCenterModules.emplace_back(MODULE::Module(0, 90, loadedParkourDifficulty, MODULE::ModuleType::CENTER, fileName, 0, 0, json));
                }
            }
        }

        // Sort modules by parkour difficulty
        SortModules(generator);
    }

    int FindConnection(MAP_GENERATOR::MG generator, int mainIndex, int& branchHeight, int rangeMinDiagonal, int rangeMaxDiagonal, int rangeMinFlat, int rangeMaxFlat)
    {
        if (branchHeight >= generator->modifiers.levelLength)
        {
            return 0;
        }

        int dif = 0;
        if (!generator->_generatedLevelSideBranch.empty() && generator->_generatedLevelSideBranch.back().type == MODULE::ModuleType::DIAGONAL_MODULE)
        {
            dif++;
        }
        if (mainIndex < generator->_generatedLevelMainBranch.size() && generator->_generatedLevelMainBranch[mainIndex].type == MODULE::ModuleType::DIAGONAL_MODULE)
        {
            dif--;
        }

        MODULE::Module module;
        int index;
        int value = 0;

        DEBUG { spdlog::info("FindConnection called with dif = {0}", dif); }

        switch (dif)
        {
            case -1:
                index = Random::get(rangeMinDiagonal, rangeMaxDiagonal);
                module = generator->_loadedDiagonalModules[index];
                module.moduleHeight = branchHeight;
                module.direction = MODULE::ModuleDirection::CW;
                if (!generator->_generatedLevelSideBranch.empty())
                {
                    module.rotation = (generator->_generatedLevelSideBranch.back().rotation + 270) % 360;
                }
                branchHeight++;
                generator->_generatedLevelSideBranch.emplace_back(module);
                if (branchHeight == generator->modifiers.levelLength)
                {
                    return 0;
                }
                else
                {
                    if (mainIndex + 1 >= generator->_generatedLevelMainBranch.size())
                    {
                        return 0;
                    }
                }

                if (mainIndex + 1 < generator->_generatedLevelMainBranch.size() && generator->_generatedLevelMainBranch[mainIndex + 1].type == MODULE::ModuleType::FLAT_MODULE)
                {
                    return 1;
                }
                else
                {
                    index = Random::get(rangeMinFlat, rangeMaxFlat);
                    module = generator->_loadedFlatModules[index];
                    module.moduleHeight = branchHeight;
                    module.direction = MODULE::ModuleDirection::CW;
                    if (!generator->_generatedLevelSideBranch.empty())
                    {
                        module.rotation = (generator->_generatedLevelSideBranch.back().rotation + 270) % 360;
                    }
                    generator->_generatedLevelSideBranch.emplace_back(module);
                    return 0;
                }
            case 1:
                if (mainIndex + 2 < generator->_generatedLevelMainBranch.size() && generator->_generatedLevelMainBranch[mainIndex + 2].type == MODULE::ModuleType::DIAGONAL_MODULE)
                {
                    index = Random::get(rangeMinFlat, rangeMaxFlat);
                    module = generator->_loadedFlatModules[index];
                    module.moduleHeight = branchHeight;
                    module.direction = MODULE::ModuleDirection::CW;
                    if (!generator->_generatedLevelSideBranch.empty())
                    {
                        module.rotation = (generator->_generatedLevelSideBranch.back().rotation + 270) % 360;
                    }
                    generator->_generatedLevelSideBranch.emplace_back(module);
                    return 1;
                }
                else
                {
                    return 2;
                }
            case 0:
                if (mainIndex + 1 < generator->_generatedLevelMainBranch.size() && generator->_generatedLevelMainBranch[mainIndex + 1].type == MODULE::ModuleType::DIAGONAL_MODULE)
                {
                    index = Random::get(rangeMinDiagonal, rangeMaxDiagonal);
                    module = generator->_loadedDiagonalModules[index];
                    module.moduleHeight = branchHeight;
                    module.direction = MODULE::ModuleDirection::CW;
                    if (!generator->_generatedLevelSideBranch.empty())
                    {
                        module.rotation = (generator->_generatedLevelSideBranch.back().rotation + 270) % 360;
                    }
                    branchHeight++;
                }
                else
                {
                    index = Random::get(rangeMinFlat, rangeMaxFlat);
                    module = generator->_loadedFlatModules[index];
                    module.moduleHeight = branchHeight;
                    module.direction = MODULE::ModuleDirection::CW;
                    if (!generator->_generatedLevelSideBranch.empty())
                    {
                        module.rotation = (generator->_generatedLevelSideBranch.back().rotation + 270) % 360;
                    }
                }
                generator->_generatedLevelSideBranch.emplace_back(module);

                return 1;
            default:
                break;
        }
        return 0; // Default return statement to handle all code paths
    }

    void GenerateCenter(MAP_GENERATOR::MG& generator)
    {
        // Randomize generation in the future?
        for (int i = 0; i < generator->modifiers.levelLength; i++)
        {
            generator->_generatedLevelCenter.emplace_back(generator->_loadedCenterModules[0]);
            generator->_generatedLevelCenter[i].moduleHeight = i;
        }
    }

    void GenerateLevel(MAP_GENERATOR::MG& generator)
    {
        int lastExitSide = 0;
        std::vector<std::string> loadedModules;

        int rangePositionDiagonal = round((generator->_loadedDiagonalModules.size() - 1) * generator->modifiers.parkourDifficulty.rangePosition);
        int rangeMinDiagonal = rangePositionDiagonal - round(generator->_loadedDiagonalModules.size() * generator->modifiers.parkourDifficulty.rangeWidth / 2);
        rangeMinDiagonal = std::max(rangeMinDiagonal, 0);
        int rangeMaxDiagonal = rangePositionDiagonal + round((generator->_loadedDiagonalModules.size()) * generator->modifiers.parkourDifficulty.rangeWidth / 2);
        rangeMaxDiagonal = std::min(rangeMaxDiagonal, int(generator->_loadedDiagonalModules.size() - 1));

        int rangePositionFlat = round((generator->_loadedFlatModules.size() - 1) * generator->modifiers.parkourDifficulty.rangePosition);
        int rangeMinFlat = rangePositionFlat - round(generator->_loadedFlatModules.size() * generator->modifiers.parkourDifficulty.rangeWidth / 2);
        rangeMinFlat = std::max(rangeMinFlat, 0);
        int rangeMaxFlat = rangePositionFlat + round((generator->_loadedFlatModules.size()) * generator->modifiers.parkourDifficulty.rangeWidth / 2);
        rangeMaxFlat = std::min(rangeMaxFlat, int(generator->_loadedFlatModules.size() - 1));

        int branchHeight = 0;
        MODULE::Module lastModule;
        lastModule.type = MODULE::ModuleType::FLAT_MODULE;
        lastModule.rotation = 270;
        if (rangeMaxDiagonal - rangeMinDiagonal + 1 < generator->modifiers.levelLength || rangeMaxFlat - rangeMinFlat + 1 < generator->modifiers.levelLength)
        {
            DEBUG { spdlog::info("Not enough modules in the selected range to generate a level"); }
            return;
        }
        else
        {
            // Main branch
            while (branchHeight < generator->modifiers.levelLength)
            {
                bool diagonalModule = Random::get<bool>(generator->modifiers.diagonalModuleProbability);
                MODULE::Module module;

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

                if (module.direction != MODULE::ModuleDirection::CW && lastModule.direction != MODULE::ModuleDirection::CW)
                {
                    module.rotation = (lastModule.rotation + 90) % 360;
                }
                else if (module.direction == MODULE::ModuleDirection::CW && lastModule.direction == MODULE::ModuleDirection::CW)
                {
                    module.rotation = (lastModule.rotation + 270) % 360;
                }

                generator->_generatedLevelMainBranch.emplace_back(module);
                loadedModules.emplace_back(module.fileName);
                lastModule = module;
            }

            branchHeight = 0;
            float branchProbability = 0.f;

            int mainIndex = 0;

            // Side branch
            while (branchHeight < generator->modifiers.levelLength && mainIndex < generator->_generatedLevelMainBranch.size())
            {
                bool diagonalModule = Random::get<bool>(generator->modifiers.diagonalModuleProbability);
                bool generateSideBranch = Random::get<bool>(branchProbability);
                MODULE::Module module;

                if (!generateSideBranch)
                {
                    if (mainIndex < generator->_generatedLevelMainBranch.size() && generator->_generatedLevelMainBranch[mainIndex].type == MODULE::ModuleType::DIAGONAL_MODULE)
                    {
                        branchHeight++;
                    }
                    branchProbability += generator->modifiers.sideBranchProbabilityStep;
                    if (branchProbability > 1.f)
                    {
                        branchProbability = 1.f;
                    }
                }
                else
                {
                    if (mainIndex > 0 && !diagonalModule && generator->_generatedLevelMainBranch[mainIndex - 1].type != MODULE::ModuleType::FLAT_MODULE)
                    {
                        int index = Random::get(rangeMinFlat, rangeMaxFlat);
                        module = generator->_loadedFlatModules[index];
                        module.moduleHeight = branchHeight;
                    }
                    else
                    {
                        int index = Random::get(rangeMinDiagonal, rangeMaxDiagonal);
                        module = generator->_loadedDiagonalModules[index];
                        module.moduleHeight = branchHeight;
                        branchHeight++;
                    }

                    module.direction = MODULE::ModuleDirection::CW;
                    if (mainIndex < generator->_generatedLevelMainBranch.size())
                    {
                        module.rotation = (generator->_generatedLevelMainBranch[mainIndex].rotation + 270) % 360;
                    }

                    generator->_generatedLevelSideBranch.emplace_back(module);

                    mainIndex += FindConnection(generator, mainIndex, branchHeight, rangeMinDiagonal, rangeMaxDiagonal, rangeMinFlat, rangeMaxFlat);
                    branchProbability = 0;
                }
                mainIndex++;
            }

            GenerateCenter(generator);

            DEBUG { spdlog::info("Generated level: "); }

            for (int i = 0; i < generator->_generatedLevelMainBranch.size(); i++)
            {
                DEBUG { spdlog::info("Main branch {0}: {1}; Difficulty: {2}; Module type: {3}; Height: {4}; Rotation: {5}", i, generator->_generatedLevelMainBranch[i].fileName, generator->_generatedLevelMainBranch[i].parkourDifficulty, MODULE::ModuleTypeToString(generator->_generatedLevelMainBranch[i].type), generator->_generatedLevelMainBranch[i].moduleHeight, generator->_generatedLevelMainBranch[i].rotation); }
            }

            for (int i = 0; i < generator->_generatedLevelSideBranch.size(); i++)
            {
                DEBUG { spdlog::info("Side branch {0}: {1}; Difficulty: {2}; Module type: {3}; Height: {4}; Rotation: {5}", i, generator->_generatedLevelSideBranch[i].fileName, generator->_generatedLevelSideBranch[i].parkourDifficulty, MODULE::ModuleTypeToString(generator->_generatedLevelSideBranch[i].type), generator->_generatedLevelSideBranch[i].moduleHeight, generator->_generatedLevelSideBranch[i].rotation); }
            }
        }
    }

    void SemiEvenSpacingGeneration (std::vector<bool>& list, int amount, float evenProbability = 0.5f)
    {
        std::vector<int> generatedIndices;
        std::vector<int> distances;
        distances.resize(list.size());

        for (int i = 0; i < amount; i++)
        {
            bool evenSpace = Random::get<bool>(evenProbability);
            if (evenSpace)
            {
                // Finding a point as evenly spaced from the others as possible
                int maxDistance = 0;
                std::vector<int> furthestDistances;

                for (int j = 0; j < distances.size(); j++)
                {
                    // For each element, calculate distance to the closest point (true)
                    int pointDistance = distances.size();
                    if (generatedIndices.empty())
                    {
                        pointDistance = 0;
                    }
                    for (int generatedIndex : generatedIndices)
                    {
                        if (fabs(j-generatedIndex) < pointDistance)
                        {
                            pointDistance = abs(j-generatedIndex);
                        }
                    }
                    distances[j] = pointDistance;

                    // If calculated distance is > maximum, it becomes new maximum
                    if (pointDistance > maxDistance)
                    {
                        furthestDistances.clear();
                        furthestDistances.emplace_back(j);
                        maxDistance = pointDistance;
                    }
                    // If calculated distance is == maximum, add it to the list
                    else if (pointDistance == maxDistance)
                    {
                        furthestDistances.emplace_back(j);
                    }
                }

                // Get a random point from the furthest distances
                int index = Random::get(0, int(furthestDistances.size() - 1));
                list[furthestDistances[index]] = true;
                generatedIndices.emplace_back(furthestDistances[index]);
            }
            else
            {
                // Get a random point, but we include only empty (false) points
                int index = Random::get(0, int(list.size() - generatedIndices.size() - 1));
                int count = 0;
                for (int j = 0; j < list.size() && count < index + 1; j++)
                {
                    if (!list[j])
                    {
                        if (count == index)
                        {
                            list[j] = true;
                            generatedIndices.emplace_back(j);
                            break;
                        }
                        count++;
                    }
                }
            }
        }
    }

    void EvenSpacingGeneration (std::vector<bool>& list, int spacing)
    {
        int space = 0;
        for (auto && i : list)
        {
            if (space == spacing)
            {
                i = true;
                space = 0;
            }
            else
            {
                space ++;
            }
        }
    }

    void TrapGeneration (MAP_GENERATOR::MG& generator, int springTrapCount)
    {
        generator->_generatedSpringTraps.resize(springTrapCount);

        SemiEvenSpacingGeneration(generator->_generatedSpringTraps, int(generator->modifiers.stationaryTrapsAmount*float(springTrapCount)));
    }

    void CheckpointsGeneration (MAP_GENERATOR::MG& generator, int checkpointsCount)
    {
        generator->_generatedCheckpoints.resize(checkpointsCount);

        EvenSpacingGeneration(generator->_generatedCheckpoints, generator->modifiers.checkpointsSpacing);
    }

    void ApplyTraps (MAP_GENERATOR::MG& generator, COLLIDER::Collider* colliders, u16 collidersCount, SCENE::World* segmentWorlds)
    {
        int index = 0;
        for (int i = 0; i < collidersCount; i++)
        {
            auto& collider = colliders[i].local;
            if (collider.collisionEventName == "SpringTrap")
            {
                if (!generator->_generatedSpringTraps[index])
                {
                    collider.isEnabled = false;
                    segmentWorlds[collider.segmentIndex].lTransforms[collider.transformIndex].base.position.y = -1000.f;
                    segmentWorlds[collider.segmentIndex].lTransforms[collider.transformIndex].flags = TRANSFORM::DIRTY;
                }
                index++;
            }
        }
    }

    void ApplyCheckpoints (MAP_GENERATOR::MG& generator, COLLIDER::Collider* colliders, u16 collidersCount, SCENE::World* segmentWorlds)
    {
        int index = 0;
        for (int i = 0; i < collidersCount; i++)
        {
            auto& collider = colliders[i].local;
            if (collider.collisionEventName == "CheckPoint")
            {
                if (!generator->_generatedCheckpoints[index])
                {
                    collider.isEnabled = false;
                    segmentWorlds[collider.segmentIndex].lTransforms[collider.transformIndex].base.position.y = -1000.f;
                    segmentWorlds[collider.segmentIndex].lTransforms[collider.transformIndex].flags = TRANSFORM::DIRTY;
                }
                index++;
            }
        }
    }

    MODULE::Module FindModule (MAP_GENERATOR::MG& generator, std::string filename)
    {
        if (filename.ends_with("0"))
        {
            for (int i = 0; i < generator->_loadedDiagonalModules.size(); i++)
            {
                if (generator->_loadedDiagonalModules[i].fileName == filename)
                {
                    return generator->_loadedDiagonalModules[i];
                }
            }
        }
        else
        {
            for (int i = 0; i < generator->_loadedFlatModules.size(); i++)
            {
                if (generator->_loadedFlatModules[i].fileName == filename)
                {
                    return generator->_loadedFlatModules[i];
                }
            }
        }
    }

    void PredefinedLevel1(MAP_GENERATOR::MG& generator, std::vector<MODULE::Module>& mainBranch, std::vector<MODULE::Module>& sideBranch)
    {
        MODULE::Module module;

        module = FindModule(generator, "t_1_0");
        module.rotation = 0;
        module.moduleHeight = 0;
        mainBranch.emplace_back(module);

        module = FindModule(generator, "t_5_1");
        module.rotation = 90;
        module.moduleHeight = 1;
        mainBranch.emplace_back(module);

        module = FindModule(generator, "t_4_0");
        module.rotation = 180;
        module.moduleHeight = 1;
        mainBranch.emplace_back(module);


//        module = FindModule(generator, "t_8_0");
//        module.rotation = 90;
//        module.moduleHeight = 1;
//        module.direction = MODULE::ModuleDirection::CW;
//        sideBranch.emplace_back(module);

    }

    void GenerateSpecificLevel(MAP_GENERATOR::MG& generator, int levelLenght)
    {
        std::vector<MODULE::Module> mainBranch;
        std::vector<MODULE::Module> sideBranch;

        generator->modifiers.levelLength = levelLenght;

        PredefinedLevel1(generator, mainBranch, sideBranch);

        for (MODULE::Module module : mainBranch)
        {
            generator->_generatedLevelMainBranch.emplace_back(module);
        }

        for (MODULE::Module module : sideBranch)
        {
            generator->_generatedLevelSideBranch.emplace_back(module);
        }

        GenerateCenter(generator);
    }
}