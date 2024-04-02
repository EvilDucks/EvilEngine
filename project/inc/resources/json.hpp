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

	const char GROUP_KEY_ALL[] { "screen\0canvas\0world" };
	const u8 KEYS_COUNT = 3;
	const u8 KEY_STARTS[KEYS_COUNT] { 0, 7, 14 };


	void FreeMaterials (
		/* IN  */ u8* sMaterialMeshTable,
		/* IN  */ u8* cMaterialMeshTable,
		/* IN  */ u8* wMaterialMeshTable
	) {
		// It is allocated using only one malloc therefore only one free is needed.
		delete[] sMaterialMeshTable;
	}

	// DO NOT USE OUTSIDE LoadMaterials function!
	void ReadMaterialsGroup (
		const char* const groupKey,
		Json& json,
		u8* materialsMeshTable
	) {
		auto& materialsCount = materialsMeshTable[0];
		u8 globalCounter = 0;

		for (u8 i = 0; i < json[groupKey].size(); ++i) {
			Json& material = json[groupKey][i];
			auto& meshesCount = materialsMeshTable[1 + i + globalCounter]; 

			// Increment materials byte.
			++materialsCount;

			for (u8 j = 0; j < material["meshes_id"].size(); ++j) {
				Json& mesh = material["meshes_id"][j];
				auto& meshId = materialsMeshTable[2 + i + globalCounter + j];

				// Assign mesh_id byte value.
				meshId = mesh;
				// Increment meshes byte.
				++meshesCount;
			}

			globalCounter += meshesCount;
		}
	}


	void LoadMaterials (
		/* OUT */ u8* sMaterialMeshTable,
		/* OUT */ u64 sMaterialsCount,
		/* OUT */ MATERIAL::Material* sMaterials,
		/* OUT */ u8* cMaterialMeshTable,
		/* OUT */ u64 cMaterialsCount,
		/* OUT */ MATERIAL::Material* cMaterials,
		/* OUT */ u8* wMaterialMeshTable,
		/* OUT */ u64 wMaterialsCount,
		/* OUT */ MATERIAL::Material* wMaterials
	) {
		std::ifstream file;
		Json json;
		
		u8 materialsMeshesBufforSize[KEYS_COUNT] { 1, 1, 1 }; // 1 - Number of materials byte
		u8 allmaterialsMeshesBufforSize = 0;
		
		DEBUG { spdlog::info ("JSON Materials Initialization"); }
		
		file.open ( "res/data/materials.json" );
		file >> json; // Parse the file.
		
		// 1. Get buffor size.
		for (u8 i = 0; i < KEYS_COUNT; ++i) {
			for (Json& object : json[GROUP_KEY_ALL + KEY_STARTS[i]]) {
				Json& shader = object["shader_id"];
				++materialsMeshesBufforSize[i];	// Number of meshes byte (each material has one)
				for (Json& mesh : object["meshes_id"]) {
					++materialsMeshesBufforSize[i]; // Mesh byte
				}
			}
			// Count up the whole buffor size.
			allmaterialsMeshesBufforSize += materialsMeshesBufforSize[i];
		}

		// Allocate the memory and assign pointers.
		sMaterialMeshTable = (u8*) calloc (allmaterialsMeshesBufforSize, sizeof (u8));
		cMaterialMeshTable = sMaterialMeshTable + materialsMeshesBufforSize[0];
		wMaterialMeshTable = cMaterialMeshTable + materialsMeshesBufforSize[1];

		// Deallocate unused.
		file.close();
		
		// finalnie powinno wynosić: { 2 {0, 1}, {1, 0} };
		//
		// Trzeba przypisać a) ilość materiałów

		//spdlog::info ("I: {0}", json.size());
		//for (Json::iterator i = json.begin(); i != json.end(); ++i) {
		//	//spdlog::info ("I: {0}", i);
		//	//std::cout << *i << '\n';
		//}

		//for (Json& object : json[GROUP_KEY_SCREEN]) {
		//	auto& materialsCount = sMaterialMeshTable[0];
		//	Json& shader = object["shader_id"];
		//	for (Json& mesh : object["meshes_id"]) {
		//	}
		//}

		// 0 - materials
		// 1 - meshes
		// 2 - mesh_id
		// 3 - meshes
		// 4 - mesh_id
		// 0, 1, 2, 3, 4

		ReadMaterialsGroup (GROUP_KEY_SCREEN, json, sMaterialMeshTable);
		ReadMaterialsGroup (GROUP_KEY_CANVAS, json, cMaterialMeshTable);
		ReadMaterialsGroup (GROUP_KEY_WORLD, json, wMaterialMeshTable);

		//for (u8 i = 0; i < materialsMeshesBufforSize[0]; ++i) {
		//	spdlog::info ("v: {0}", sMaterialMeshTable[i]);
		//}
		//for (u8 i = 0; i < materialsMeshesBufforSize[1]; ++i) {
		//	spdlog::info ("v: {0}", cMaterialMeshTable[i]);
		//}
		//for (u8 i = 0; i < materialsMeshesBufforSize[2]; ++i) {
		//	spdlog::info ("v: {0}", wMaterialMeshTable[i]);
		//}

		//spdlog::info ("a: {0}", (u8)materialsCount);
		//spdlog::info ("b: {0}", (u8)globalCounter);
		//spdlog::info ("c: {0}", (u8)meshesCounter);

		//spdlog::info ("Buffor Size: {0}", materialsMeshesBufforSize[0]); // 5 (ilość mat) { (ilość meshy) ...meshe... }, { ... }
		//spdlog::info ("Buffor Size: {0}", materialsMeshesBufforSize[1]); // 1
		//spdlog::info ("Buffor Size: {0}", materialsMeshesBufforSize[2]); // 4
	}

}