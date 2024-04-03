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


	void DestoryMaterials (
		/* IN  */ MATERIAL::MaterialMeshTable* sMaterialMeshTable,
		/* IN  */ MATERIAL::Material* sMaterials,
		//
		/* IN  */ MATERIAL::MaterialMeshTable* cMaterialMeshTable,
		/* IN  */ MATERIAL::Material* cMaterials,
		//
		/* IN  */ MATERIAL::MaterialMeshTable* wMaterialMeshTable,
		/* IN  */ MATERIAL::Material* wMaterials
	) {
		// It is allocated using only one malloc therefore only one free is needed.
		delete[] sMaterialMeshTable;
		delete[] sMaterials;
		delete[] cMaterials;
		delete[] wMaterials;
	}


	// DO NOT USE OUTSIDE LoadMaterials function!
	void ReadMaterialsGroup (
		const char* const groupKey,
		Json& json,
		u8* materialsMeshTable,
		u64& materialsCount,
		MATERIAL::Material* materials
	) {
		auto& materialsCounter = materialsMeshTable[0];
		u8 globalCounter = 0;

		// For each material.
		for (u8 i = 0; i < json[groupKey].size(); ++i) {
			Json& material = json[groupKey][i];
			auto& meshesCount = materialsMeshTable[1 + i + globalCounter];

			// Increment materials byte.
			++materialsCounter;

			// For each mesh ref in material.
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

		materialsCount = materialsCounter;
	}


	void GetBufforSize (
		/* IN  */ const char* const groupKey,
		/* OUT */ Json& json,
		/* OUT */ u8& materialsMeshesBufforSize,
		/* OUT */ u64& materialsCounter
	) {
		for (; materialsCounter < json[groupKey].size(); ++materialsCounter) {
			Json& material = json[groupKey][materialsCounter];
			Json& shader = material["shader_id"];
			++materialsMeshesBufforSize;	// Number of meshes byte (each material has one)
			for (Json& mesh : material["meshes_id"]) {
				++materialsMeshesBufforSize; // Mesh byte
			}
		}
	}


	void CreateMaterials (
		/* OUT */ Json& json,
		//
		/* OUT */ MATERIAL::MaterialMeshTable*& sMaterialMeshTable,
		/* OUT */ u64& sMaterialsCount,
		/* OUT */ MATERIAL::Material*& sMaterials,
		//
		/* OUT */ MATERIAL::MaterialMeshTable*& cMaterialMeshTable,
		/* OUT */ u64& cMaterialsCount,
		/* OUT */ MATERIAL::Material*& cMaterials,
		//
		/* OUT */ MATERIAL::MaterialMeshTable*& wMaterialMeshTable,
		/* OUT */ u64& wMaterialsCount,
		/* OUT */ MATERIAL::Material*& wMaterials
	) {
		std::ifstream file;
		
		// We initialize it with 1 because theres 1 byte representing materials count.
		u8 materialsMeshesBufforSize[KEYS_COUNT] { 1, 1, 1 }; 
		u8 allmaterialsMeshesBufforSize = 0;
		
		DEBUG { spdlog::info ("JSON Materials Initialization"); }
		
		file.open ( "res/data/materials.json" );
		file >> json; // Parse the file.

		// Make sure counters are initialized and set to 0.
		sMaterialsCount = 0;
		cMaterialsCount = 0;
		wMaterialsCount = 0;
		
		// Count up all the materials from each group.
		GetBufforSize (GROUP_KEY_ALL + KEY_STARTS[0], json, materialsMeshesBufforSize[0], sMaterialsCount);
		GetBufforSize (GROUP_KEY_ALL + KEY_STARTS[1], json, materialsMeshesBufforSize[1], cMaterialsCount);
		GetBufforSize (GROUP_KEY_ALL + KEY_STARTS[2], json, materialsMeshesBufforSize[2], wMaterialsCount);
		
		// Count up the whole buffor size for MaterialMeshTable too.
		for (u8 i = 0; i < KEYS_COUNT; ++i) {
			allmaterialsMeshesBufforSize += materialsMeshesBufforSize[i];
		}

		// Deallocate unused.
		file.close();

		// Allocate the memory and assign pointers.
		sMaterialMeshTable = (u8*) calloc (allmaterialsMeshesBufforSize, sizeof (u8));
		cMaterialMeshTable = sMaterialMeshTable + materialsMeshesBufforSize[0];
		wMaterialMeshTable = cMaterialMeshTable + materialsMeshesBufforSize[1];

		// If not 0 allocate !
		if (sMaterialsCount) sMaterials = new MATERIAL::Material[sMaterialsCount];
		if (cMaterialsCount) cMaterials = new MATERIAL::Material[cMaterialsCount];
		if (wMaterialsCount) wMaterials = new MATERIAL::Material[wMaterialsCount];
	}


	void LoadMaterials (
		/* IN  */ Json& json,
		//
		/* OUT */ MATERIAL::MaterialMeshTable*& sMaterialMeshTable,
		/* OUT */ u64& sMaterialsCount,
		/* OUT */ MATERIAL::Material* sMaterials,
		//
		/* OUT */ MATERIAL::MaterialMeshTable*& cMaterialMeshTable,
		/* OUT */ u64& cMaterialsCount,
		/* OUT */ MATERIAL::Material* cMaterials,
		//
		/* OUT */ MATERIAL::MaterialMeshTable*& wMaterialMeshTable,
		/* OUT */ u64& wMaterialsCount,
		/* OUT */ MATERIAL::Material* wMaterials
	) {
		ReadMaterialsGroup (GROUP_KEY_SCREEN, json, sMaterialMeshTable, sMaterialsCount, sMaterials);
		ReadMaterialsGroup (GROUP_KEY_CANVAS, json, cMaterialMeshTable, cMaterialsCount, cMaterials);
		ReadMaterialsGroup (GROUP_KEY_WORLD, json, wMaterialMeshTable, wMaterialsCount, wMaterials);
	}

	// 0 - materials
	// 1 - meshes
	// 2 - mesh_id
	// 3 - meshes
	// 4 - mesh_id
	// 0, 1, 2, 3, 4
	// Creates Links Material -> Mesh/es 

}