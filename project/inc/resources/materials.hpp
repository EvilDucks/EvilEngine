#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

namespace RESOURCES::MATERIALS {

	const char GROUP_KEY_SCREEN[] = "screen";
	const char GROUP_KEY_CANVAS[] = "canvas";
	const char GROUP_KEY_WORLD[] = "world";

	const char GROUP_KEY_ALL[] { "screen\0canvas\0world" };
	const u8 KEYS_COUNT = 3;
	const u8 KEY_STARTS[KEYS_COUNT] { 0, 7, 14 };


	void DestoryMaterials (
		/* IN  */ u8* sUniformsTable,
		/* IN  */ MATERIAL::MaterialMeshTable* sMaterialMeshTable,
		/* IN  */ MATERIAL::Material* sMaterials,
		//
		/* IN  */ u8* cUniformsTable,
		/* IN  */ MATERIAL::MaterialMeshTable* cMaterialMeshTable,
		/* IN  */ MATERIAL::Material* cMaterials,
		//
		/* IN  */ u8* wUniformsTable,
		/* IN  */ MATERIAL::MaterialMeshTable* wMaterialMeshTable,
		/* IN  */ MATERIAL::Material* wMaterials
	) {
		// It is allocated using only one malloc therefore only one free is needed.
		delete[] sMaterialMeshTable;
		delete[] sUniformsTable;
		// These are normal.
		delete[] sMaterials;
		delete[] cMaterials;
		delete[] wMaterials;
	}


	// DO NOT USE OUTSIDE LoadMaterials function!
	void ReadMaterialsGroup (
		/* IN  */ const char* const groupKey,
		/* IN  */ Json& json,
		/* IN  */ u8* uniformsTable,
		/* IN  */ u8* materialsMeshTable,
		/* OUT */ u64& materialsCount,
		/* ??? */ MATERIAL::Material* materials
	) {
		const u8 UNIFORM_BYTES_COUNT = sizeof (SHADER::UNIFORM::Uniform);

		auto& materialsCounterA = materialsMeshTable[0];
		auto& materialsCounterB = uniformsTable[0];

		MATERIAL::MESHTABLE::SetRead (0);
		u16 uniformsTableBytesRead = 0;
		
		materialsCount = json[groupKey].size();
		materialsCounterA = materialsCount;
		materialsCounterB = materialsCount;

		for (u8 iMaterial = 0; iMaterial < materialsCount; ++iMaterial) {
			Json& material = json[groupKey][iMaterial];

			auto& meshesCount = *MATERIAL::MESHTABLE::GetMeshCount (materialsMeshTable, iMaterial);
			auto& uniformsCount = *SIZED_BUFFOR::GetCount (uniformsTable, iMaterial, uniformsTableBytesRead);

			Json& shader = material["shader"];

			if ( shader.contains ("uniforms") ) {

				Json& uniforms = shader["uniforms"];
				uniformsCount = uniforms.size();

				for (u8 iUniform = 0; iUniform < uniformsCount; ++iUniform) {
					Json& uniform = uniforms[iUniform];

					// I hate everything vol1.
					std::string temp = uniform.get<std::string>();
					const char* uniformName = temp.c_str();

					auto& uniformNamesCount = SHADER::UNIFORM::NAMES::namesCount;
					auto& uniformNames = SHADER::UNIFORM::NAMES::names;

					u8 iUniformName = 0;

					// !!! ERRORS !!! THIS DOES NOT CHECK IF JSON IS VALID !!!
					for (; strcmp (uniformName, uniformNames[iUniformName]) != 0; ++iUniformName);

					auto& matchedUniform = SHADER::UNIFORM::uniforms[iUniformName];

					auto&& uniformTable = (SHADER::UNIFORM::Uniform*)(
						uniformsTable + 2 + uniformsTableBytesRead + iMaterial + (UNIFORM_BYTES_COUNT * iUniform)
					);

					// Writes matched uniform to the specified memory location.
					*uniformTable = matchedUniform;

					//DEBUG_FILE spdlog::info("a: {0}, {1}", iMaterial, 2 + uniformsTableBytesRead + iMaterial + (UNIFORM_BYTES_COUNT * iUniform));
					//DEBUG_FILE spdlog::info ("c: {0}, {1}", uniformName, iUniformName);
				}

			} else {
				// Is that needed ?
				//spdlog::info ("c: nope");
				uniformsCount = 0;
			}

			Json& meshes = material["meshes_id"];
			meshesCount = meshes.size();

			for (u8 iMesh = 0; iMesh < meshesCount; ++iMesh) {
				Json& mesh = material["meshes_id"][iMesh];
				auto& meshId = *MATERIAL::MESHTABLE::GetMesh (materialsMeshTable, iMaterial, iMesh);
				meshId = (u8)mesh;	// Assign mesh_id byte value.
			}

			MATERIAL::MESHTABLE::AddRead (meshesCount);
			uniformsTableBytesRead += uniformsCount * UNIFORM_BYTES_COUNT;
		}
		MATERIAL::MESHTABLE::SetRead (0);
	}


	void GetBufforSize (
		/* IN  */ const char* const groupKey,
		/* IN  */ Json& json,
		/* OUT */ u32& uniformsTableBytes,
		/* OUT */ u8& materialsMeshesBufforSize,
		/* OUT */ u64& materialsCounter
	) {
		const u8 UNIFORM_BYTES_COUNT = sizeof (SHADER::UNIFORM::Uniform);

		for (; materialsCounter < json[groupKey].size(); ++materialsCounter) {
			Json& material = json[groupKey][materialsCounter];

			DEBUG_FILE if (material == nullptr) { 
				spdlog::error (ERROR_CONTAIN, "materials.json", "material");
				exit (1);
			}

			if ( material.contains ("shader") ) {
				Json& shader = material["shader"];

				if ( shader.contains ("uniforms") ) {
					Json& uniforms = shader["uniforms"];

					// We need 1 byte to define a new material followed by it's uniform's bytes.
					//  This also triggers when ' "uniforms": {-} ' which is good.
					uniformsTableBytes += 1 + (UNIFORM_BYTES_COUNT * uniforms.size());
				} else {
					// When there's no uniforms defined we place an empty byte to recognize that.
					uniformsTableBytes += 1;
				}

			} else {

				DEBUG_FILE { 
					spdlog::error ("One of materials in JSON file is missing it's shader declaration!");
					exit (1);
				}

			}

			++materialsMeshesBufforSize;	// Number of meshes byte (each material has one)
			for (Json& mesh : material["meshes_id"]) {
				++materialsMeshesBufforSize; // Mesh byte
			}
		}
	}


	void CreateMaterials (
		/* OUT */ Json& json,
		//
		/* OUT */ u8*& sUniformsTable,
		/* OUT */ MATERIAL::MaterialMeshTable*& sMaterialMeshTable,
		/* OUT */ u64& sMaterialsCount,
		/* OUT */ MATERIAL::Material*& sMaterials,
		//
		/* OUT */ u8*& cUniformsTable,
		/* OUT */ MATERIAL::MaterialMeshTable*& cMaterialMeshTable,
		/* OUT */ u64& cMaterialsCount,
		/* OUT */ MATERIAL::Material*& cMaterials,
		//
		/* OUT */ u8*& wUniformsTable,
		/* OUT */ MATERIAL::MaterialMeshTable*& wMaterialMeshTable,
		/* OUT */ u64& wMaterialsCount,
		/* OUT */ MATERIAL::Material*& wMaterials
	) {
		std::ifstream file;
		
		// We initialize it with 1 because theres 1 byte representing materials count.
		u8 materialsMeshesBufforSize[KEYS_COUNT] { 1, 1, 1 }; 
		u8 allMaterialsMeshesBufforSize = 0;

		u32 uniformsTableBytes[KEYS_COUNT] { 1, 1, 1 }; 
		u32 allUniformsTableBytes = 0;
		
		DEBUG { spdlog::info ("JSON Materials Initialization"); }
		
		file.open ( "res/data/materials.json" );
		file >> json; // Parse the file.

		// Make sure counters are initialized and set to 0.
		sMaterialsCount = 0;
		cMaterialsCount = 0;
		wMaterialsCount = 0;
		
		// Count up all the materials from each group.
		GetBufforSize (GROUP_KEY_ALL + KEY_STARTS[0], json, uniformsTableBytes[0], materialsMeshesBufforSize[0], sMaterialsCount);
		GetBufforSize (GROUP_KEY_ALL + KEY_STARTS[1], json, uniformsTableBytes[1], materialsMeshesBufforSize[1], cMaterialsCount);
		GetBufforSize (GROUP_KEY_ALL + KEY_STARTS[2], json, uniformsTableBytes[2], materialsMeshesBufforSize[2], wMaterialsCount);

		//spdlog::info ("s: {0}", uniformsTableBytes[0]);
		//spdlog::info ("c: {0}", uniformsTableBytes[1]);
		//spdlog::info ("w: {0}", uniformsTableBytes[2]);
		
		// Count up the whole buffor size for both.
		for (u8 i = 0; i < KEYS_COUNT; ++i) {
			allMaterialsMeshesBufforSize += materialsMeshesBufforSize[i];
			allUniformsTableBytes += uniformsTableBytes[i];
		}
		
		//spdlog::info ("w: {0}", allUniformsTableBytes);

		// Deallocate unused.
		file.close();

		
		{ // Allocate the memory and assign pointers.

			sMaterialMeshTable = (u8*) calloc (allMaterialsMeshesBufforSize, sizeof (u8));
			cMaterialMeshTable = sMaterialMeshTable + materialsMeshesBufforSize[0];
			wMaterialMeshTable = cMaterialMeshTable + materialsMeshesBufforSize[1];

			sUniformsTable = (u8*) calloc (allUniformsTableBytes, sizeof (u8));
			cUniformsTable = sUniformsTable + uniformsTableBytes[0];
			wUniformsTable = cUniformsTable + uniformsTableBytes[1];

			// If not 0 allocate !
			if (sMaterialsCount) sMaterials = new MATERIAL::Material[sMaterialsCount];
			if (cMaterialsCount) cMaterials = new MATERIAL::Material[cMaterialsCount];
			if (wMaterialsCount) wMaterials = new MATERIAL::Material[wMaterialsCount];

		}
		
	}


	void LoadMaterials (
		/* IN  */ Json& json,
		//
		/* OUT */ u8*& sUniformsTable,
		/* OUT */ MATERIAL::MaterialMeshTable*& sMaterialMeshTable,
		/* OUT */ u64& sMaterialsCount,
		/* OUT */ MATERIAL::Material* sMaterials,
		//
		/* OUT */ u8*& cUniformsTable,
		/* OUT */ MATERIAL::MaterialMeshTable*& cMaterialMeshTable,
		/* OUT */ u64& cMaterialsCount,
		/* OUT */ MATERIAL::Material* cMaterials,
		//
		/* OUT */ u8*& wUniformsTable,
		/* OUT */ MATERIAL::MaterialMeshTable*& wMaterialMeshTable,
		/* OUT */ u64& wMaterialsCount,
		/* OUT */ MATERIAL::Material* wMaterials
	) {
		ReadMaterialsGroup (GROUP_KEY_SCREEN, json, sUniformsTable, sMaterialMeshTable, sMaterialsCount, sMaterials);
		ReadMaterialsGroup (GROUP_KEY_CANVAS, json, cUniformsTable, cMaterialMeshTable, cMaterialsCount, cMaterials);
		ReadMaterialsGroup (GROUP_KEY_WORLD, json, wUniformsTable, wMaterialMeshTable, wMaterialsCount, wMaterials);
	}

	// 0 - materials
	// 1 - meshes
	// 2 - mesh_id
	// 3 - meshes
	// 4 - mesh_id
	// 0, 1, 2, 3, 4
	// Creates Links Material -> Mesh/es 

}