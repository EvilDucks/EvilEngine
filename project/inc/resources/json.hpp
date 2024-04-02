#pragma once
#include "types.hpp"
#include "manager.hpp"
#include "scene.hpp"

// NLOHMANN_JSON
#include <nlohmann/json.hpp>

// To simplify the process. Atleast for now.
#include <fstream>
#include <sstream>
#include <string>

namespace RESOURCES::JSON {

	using Json = nlohmann::json;

	const char GROUP_KEY_SCREEN[] = "screen";
	const char GROUP_KEY_CANVAS[] = "canvas";
	const char GROUP_KEY_WORLD[] = "world";


	void Close (
		/* IN  */ std::ifstream& file
	) {
		file.close();
	}


	void OpenMaterials (
		/* OUT */ std::ifstream& file,
		/* OUT */ Json& json
	) {
		file.open ( "res/data/materials.json" );
		file >> json; // Parse the file.
	}


	void LoadMaterials (
		/* IN  */ const char* const groupKey,
		/* IN  */ Json& json,
		/* OUT */ u8* materialMeshTable,
		/* OUT */ u64 materialsCount,
		/* OUT */ MATERIAL::Material* materials
	) {
		DEBUG { // Access fields
			spdlog::info ("JSON Materials Initialization");
			for (Json& object : json[groupKey]) {
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
	}

}