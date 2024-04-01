#pragma once
#include "types.hpp"
#include "manager.hpp"

// NLOHMANN_JSON
#include <nlohmann/json.hpp>

// To simplify the process. Atleast for now.
#include <fstream>
#include <sstream>
#include <string>

namespace RESOURCES::JSON {

    using Json = nlohmann::json;

    void LoadMaterials () {

        // Retrive Materials Tables

        // u64 screenMaterialsCount
        // u64 canvasMaterialsCount
        // u64 worldMaterialsCount
        // MATERIAL::Material screenMaterials
        // MATERIAL::Material canvasMaterials
        // MATERIAL::Material worldMaterials

        // Retrive Material-Mesh Tables
        //  these don't need count varaibles right?

        // u8* screenMaterialMeshTable
		// u8* canvasMaterialMeshTable
		// u8* worldMaterialMeshTable

        Json json;

        std::ifstream file ( "res/data/materials.json" );
        
        // Parse the file.
        file >> json;

        
        DEBUG { // Access fields
            spdlog::info ("JSON Materials Initialization");
            for (Json& object : json["screen"]) {
                Json& shader = object["shader_id"];
                //
                spdlog::info ("ShaderID: {0}", (u32)shader);
                for (Json& mesh : object["meshes_id"])
                    spdlog::info ("MeshID: {0}", (u32)mesh);
            }
            for (Json& object : json["canvas"]) {
                Json& shader = object["shader_id"];
                //
                spdlog::info ("ShaderID: {0}", (u32)shader);
                for (Json& mesh : object["meshes_id"])
                    spdlog::info ("MeshID: {0}", (u32)mesh);
            }
            for (Json& object : json["world"]) {
                Json& shader = object["shader_id"];
                //
                spdlog::info ("ShaderID: {0}", (u32)shader);
                for (Json& mesh : object["meshes_id"])
                    spdlog::info ("MeshID: {0}", (u32)mesh);
            }
        }
        
        
        file.close();
    }

}