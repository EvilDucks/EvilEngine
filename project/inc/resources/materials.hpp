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
		/* IN  */ u8* sMaterialMeshTable,
		/* IN  */ MATERIAL::Material* sMaterials,
		//
		/* IN  */ u8* cUniformsTable,
		/* IN  */ u8* cMaterialMeshTable,
		/* IN  */ MATERIAL::Material* cMaterials,
		//
		/* IN  */ u8* wUniformsTable,
		/* IN  */ u8* wMaterialMeshTable,
		/* IN  */ MATERIAL::Material* wMaterials
	) {
        ZoneScopedN("RESOURCES::MATERIALS: DestoryMaterials");

		// It is allocated using only one malloc therefore only one free is needed.
		delete[] sMaterialMeshTable;
		delete[] sUniformsTable;
		// These are normal.
		delete[] sMaterials;
		delete[] cMaterials;
		delete[] wMaterials;
	}


	void DestroyLoadShaders (
		/* IN  */ u8* shadersLoadTableBytes,
		/* IN  */ u8* chadersLoadTableBytes,
		/* IN  */ u8* whadersLoadTableBytes
	) {
		// It is allocated using only one malloc therefore only one free is needed.
		delete[] shadersLoadTableBytes;
	}


	// DO NOT USE OUTSIDE LoadMaterials function!
	void ReadMaterialsGroup (
		/* IN  */ const char* const groupKey,
		/* IN  */ Json& json,
		/* IN  */ u8* shadersLoadTable,
		/* IN  */ u8* uniformsTable,
		/* IN  */ u8* materialsMeshTable,
		/* OUT */ u8& materialsCount,
		/* ??? */ MATERIAL::Material* materials
	) {
		ZoneScopedN("RESOURCES::MATERIALS: ReadMaterialsGroup");

		auto& materialsCounterA = materialsMeshTable[0];
		auto& materialsCounterB = uniformsTable[0];
		auto& shadersCounter = shadersLoadTable[0];

		u64 shadersLoadTableBytesRead = 0;
		MATERIAL::MESHTABLE::SetRead (0);
		u16 uniformsTableBytesRead = 0;
		
		materialsCount = json[groupKey].size();

		materialsCounterA = materialsCount;
		materialsCounterB = materialsCount;
		shadersCounter = materialsCount;

		for (u8 iMaterial = 0; iMaterial < materialsCount; ++iMaterial) {
			std::string temp;

			Json& material = json[groupKey][iMaterial];

			auto& meshesCount = *MATERIAL::MESHTABLE::GetMeshCount (materialsMeshTable, iMaterial);
			auto& uniformsCount = *SIZED_BUFFOR::GetCount (uniformsTable, iMaterial, uniformsTableBytesRead);

			Json& shader = material["shader"];


			// I hate everything vol3.
			{ // Get Name
				auto&& destination = shadersLoadTable + 1 + shadersLoadTableBytesRead;
				Json& shaderName = shader["name"];

				temp = shaderName.get<std::string>();
				memcpy (destination, temp.c_str(), temp.size() * sizeof (u8) );
				shadersLoadTableBytesRead += temp.size() + 1; // null-terminate
			}
			{ // Get Vert
				auto&& destination = shadersLoadTable + 1 + shadersLoadTableBytesRead;
				Json& vert = shader["vert"];

				temp = vert.get<std::string>();
				memcpy (destination, temp.c_str(), temp.size() * sizeof (u8) );
				shadersLoadTableBytesRead += temp.size() + 1;
			}
			{ // Get Frag
				auto&& destination = shadersLoadTable + 1 + shadersLoadTableBytesRead;
				Json& frag = shader["frag"];

				temp = frag.get<std::string>();
				memcpy (destination, temp.c_str(), temp.size() * sizeof (u8) );
				shadersLoadTableBytesRead += temp.size() + 1;
			}


			if ( shader.contains ("uniforms") ) {

				Json& uniforms = shader["uniforms"];
				uniformsCount = uniforms.size();

				{	// Save Uniforms Count inside Shaders LoadTable.
					auto&& shaderUniforms = shadersLoadTable + 1 + shadersLoadTableBytesRead;
					*shaderUniforms = uniforms.size();
					shadersLoadTableBytesRead += 1;
				}
				

				for (u8 iUniform = 0; iUniform < uniformsCount; ++iUniform) {
					std::string temp;
					Json& type = uniforms[iUniform]["type"];
					

					{ // Get Uniform name.
						auto&& destination = shadersLoadTable + 1 + shadersLoadTableBytesRead;
						Json& uniformName = uniforms[iUniform]["name"];

						temp = uniformName.get<std::string>();
						memcpy (destination, temp.c_str(), temp.size() * sizeof (u8) );
						shadersLoadTableBytesRead += temp.size() + 1;
					}

					// I hate everything vol1.
					temp = type.get<std::string>();
					const char* uniformType = temp.c_str();

					auto& uniformTypesCount = SHADER::UNIFORM::NAMES::namesCount;
					auto& uniformTypes = SHADER::UNIFORM::NAMES::names;

					u8 iUniformType = 0;

					// !!! ERRORS !!! THIS DOES NOT CHECK IF JSON IS VALID !!!
					for (; strcmp (uniformType, uniformTypes[iUniformType]) != 0; ++iUniformType);

					auto& matchedUniform = SHADER::UNIFORM::uniforms[iUniformType];

					auto&& uniformTable = (SHADER::UNIFORM::Uniform*)(
						uniformsTable + 2 + uniformsTableBytesRead + iMaterial + (SHADER::UNIFORM::UNIFORM_BYTES * iUniform)
					);

					// Writes matched uniform-type to the specified memory location.
					*uniformTable = matchedUniform;

					//DEBUG_FILE spdlog::info("a: {0}, {1}", iMaterial, 2 + uniformsTableBytesRead + iMaterial + (SHADER::UNIFORM::UNIFORM_BYTES * iUniform));
					//DEBUG_FILE spdlog::info ("c: {0}, {1}", uniformType, iUniformType);
				}

			} else {
				// Is that needed ?
				//spdlog::info ("c: nope");
				uniformsCount = 0;
			}

			auto&& meshes = material["meshes_id"];
			meshesCount = meshes.size();
			
			uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;

			if (meshesCount != 0) {

				// Additional memory allocation!
				u8* tempArray = (u8*) calloc (meshesCount, sizeof (u8));

				// SORTING + ASSIGN
				Json& meshFirst = meshes[0];
				tempArray[0] = meshFirst.get<int> ();
				SortAssign (meshesCount, meshes, tempArray);

				// Get Mesh + Instances bytes 
				u8 nonDuplicates = 1;
				u8 lastDuplicateCount = 1;
				u8 lastDuplicate = tempArray[0];

				// Get trough temp array to get duplicates info and assign values properly 
				//  in meshId, instancesCount way.
				for (u8 iMesh = 1; iMesh < meshesCount; ++iMesh) {
					u8 value = tempArray[iMesh];
					++lastDuplicateCount;

					// Apply values for previous duplicate.
					if (value != lastDuplicate) { 
						//spdlog::warn ("call");

						auto& meshId = *MATERIAL::MESHTABLE::GetMesh (materialsMeshTable, iMaterial, nonDuplicates - 1);
						meshId = lastDuplicate;	// Assign mesh_id byte value.
						//spdlog::info (meshId);

						auto& instance = *MATERIAL::MESHTABLE::GetMeshInstancesCount (materialsMeshTable, iMaterial, nonDuplicates - 1);
						instance = lastDuplicateCount - 1;
						//spdlog::info (instance);

						lastDuplicateCount = 1;
						lastDuplicate = value;
						++nonDuplicates;
					}
				}

				// We used it to store tempArray values count now we want to store
				//  count of (MeshID, InstancesCount).
				meshesCount = nonDuplicates; 

				{ // Apply values for last duplicate
					//spdlog::warn (nonDuplicates - 1);
					auto& meshId = *MATERIAL::MESHTABLE::GetMesh (materialsMeshTable, iMaterial, nonDuplicates - 1);
					meshId = lastDuplicate;	// Assign mesh_id byte value.
					//spdlog::info (meshId);

					auto& instance = *MATERIAL::MESHTABLE::GetMeshInstancesCount (materialsMeshTable, iMaterial, nonDuplicates - 1);
					instance = lastDuplicateCount;
					//spdlog::info (instance);
				}

				MATERIAL::MESHTABLE::AddRead (meshesCount * 2); // 2 bytes meshID & instance

				free (tempArray);
			}
		}
		MATERIAL::MESHTABLE::SetRead (0);
	}

	void GetBufforSize (
		/* IN  */ const char* const groupKey,
		/* IN  */ Json& json,
		/* OUT */ u64& shadersLoadTableBytes,
		/* OUT */ u32& uniformsTableBytes,
		/* OUT */ u8& materialsMeshesBufforSize,
		/* OUT */ u8& materialsCounter
	) {
		ZoneScopedN("RESOURCES::MATERIALS: GetBufforSize");

		for (; materialsCounter < json[groupKey].size (); ++materialsCounter) {
			Json& material = json[groupKey][materialsCounter];

			DEBUG_FILE if (material == nullptr) { 
				spdlog::error (ERROR_CONTAIN, "materials.json", "material");
				exit (1);
			}

			if ( material.contains ("shader") ) {
				std::string temp;

				Json& shader = material["shader"];
				Json& shaderName = shader["name"];
				Json& vert = shader["vert"];
				Json& frag = shader["frag"];

				// I hate everything vol2.
				// We don't allocate a byte for defining string size. We null terminate it with an additional byte.
				temp = shaderName.get<std::string>();
				shadersLoadTableBytes += temp.size() + 1;
				temp = vert.get<std::string>();
				shadersLoadTableBytes += temp.size() + 1;
				temp = frag.get<std::string>();
				shadersLoadTableBytes += temp.size() + 1;

				if ( shader.contains ("uniforms") ) {
					Json& uniforms = shader["uniforms"];
					// We need 1 byte to define a new material followed by it's uniform's bytes.
					//  This also triggers when ' "uniforms": {-} ' which is good.
					uniformsTableBytes += 1 + (SHADER::UNIFORM::UNIFORM_BYTES * uniforms.size ());

					shadersLoadTableBytes += 1; // 1 byte to determine count of uniforms.
					for (u8 iUniform = 0; iUniform < uniforms.size (); ++iUniform) {
						Json& uniformName = uniforms[iUniform]["name"];

						// I hate everything vol4.
						temp = uniformName.get<std::string> ();
						shadersLoadTableBytes += temp.size () + 1; // null-terminate, again.
					}
					

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

			auto&& meshes = material["meshes_id"];
			auto&& meshesCount = meshes.size();

			if (meshesCount != 0) {

				// First MeshID byte + Instances byte
				materialsMeshesBufforSize += 2;

				// Additional memory allocation!
				u8* tempArray = (u8*) calloc (meshesCount, sizeof (u8));

				// SORTING + ASSIGN
				Json& meshFirst = meshes[0];
				tempArray[0] = meshFirst.get<int> ();
				SortAssign (meshesCount, meshes, tempArray);

				// Get Mesh + Instances bytes 
				u8 lastDuplicate = tempArray[0];

				for (u8 iMesh = 1; iMesh < meshesCount; ++iMesh) {
					u8 value = tempArray[iMesh];

					if (value != lastDuplicate) {
						lastDuplicate = value;
						// MeshID byte + Instances byte
						materialsMeshesBufforSize += 2;
					}
				}

				//DEBUG for (u8 iMesh = 0; iMesh < meshesCount; ++iMesh) {
				//	spdlog::info ("{0}: {1}", iMesh, tempArray[iMesh]);
				//}

				free (tempArray);

			}
		}
	}


	void CreateMaterials (
		/* OUT */ Json& json,
		//
		/* OUT */ u8*& sShadersLoadTable,
		/* OUT */ u8*& sUniformsTable,
		/* OUT */ u8*& sMeshesTable,
		/* OUT */ u8& sMaterialsCount,
		/* OUT */ MATERIAL::Material*& sMaterials,
		//
		/* OUT */ u8*& cShadersLoadTable,
		/* OUT */ u8*& cUniformsTable,
		/* OUT */ u8*& cMeshesTable,
		/* OUT */ u8& cMaterialsCount,
		/* OUT */ MATERIAL::Material*& cMaterials,
		//
		/* OUT */ u8*& wShadersLoadTable,
		/* OUT */ u8*& wUniformsTable,
		/* OUT */ u8*& wMeshesTable,
		/* OUT */ u8& wMaterialsCount,
		/* OUT */ MATERIAL::Material*& wMaterials
	) {
        ZoneScopedN("RESOURCES::MATERIALS: CreateMaterials");

		std::ifstream file;
		
		// We initialize it with 1 because theres 1 byte representing materials count.
		u8 meshesTableBytes[KEYS_COUNT] { 1, 1, 1 }; 
		u8 allMeshesTableBytes = 0;

		u32 uniformsTableBytes[KEYS_COUNT] { 1, 1, 1 }; 
		u32 allUniformsTableBytes = 0;

		u64 shadersLoadTableBytes[KEYS_COUNT] { 1, 1, 1 }; 
		u64 allShadersLoadTableBytes = 0;
		
		DEBUG { spdlog::info ("JSON Materials Initialization"); }
		
		file.open ( "res/data/materials.json" );
		file >> json; // Parse the file.

		// Make sure counters are initialized and set to 0.
		sMaterialsCount = 0;
		cMaterialsCount = 0;
		wMaterialsCount = 0;
		
		// Count up all the materials from each group.
		GetBufforSize (GROUP_KEY_ALL + KEY_STARTS[0], json, shadersLoadTableBytes[0], uniformsTableBytes[0], meshesTableBytes[0], sMaterialsCount);
		GetBufforSize (GROUP_KEY_ALL + KEY_STARTS[1], json, shadersLoadTableBytes[1], uniformsTableBytes[1], meshesTableBytes[1], cMaterialsCount);
		GetBufforSize (GROUP_KEY_ALL + KEY_STARTS[2], json, shadersLoadTableBytes[2], uniformsTableBytes[2], meshesTableBytes[2], wMaterialsCount);

		//spdlog::info ("s: {0}", uniformsTableBytes[0]);
		//spdlog::info ("c: {0}", uniformsTableBytes[1]);
		//spdlog::info ("w: {0}", uniformsTableBytes[2]);

		DEBUG spdlog::info ("mtb1: {0}", meshesTableBytes[2]);
		
		// Count up the whole buffor size for both.
		for (u8 i = 0; i < KEYS_COUNT; ++i) {
			//allMeshesTableBytes += meshesTableBytes[i];
			allUniformsTableBytes += uniformsTableBytes[i];
			allShadersLoadTableBytes += shadersLoadTableBytes[i];
		}
		
		//spdlog::info ("w: {0}", allUniformsTableBytes);

		// Deallocate unused.
		file.close();

		
		{ // Allocate the memory and assign pointers.

			// !!! Moving code to location.hpp !!! 
			//sMeshesTable = (u8*) calloc (allMeshesTableBytes, sizeof (u8));
			//cMeshesTable = sMeshesTable + meshesTableBytes[0];
			//wMeshesTable = cMeshesTable + meshesTableBytes[1];

			// TEMPORARY FIX
			allMeshesTableBytes += meshesTableBytes[0] + meshesTableBytes[1];
			sMeshesTable = (u8*) calloc (allMeshesTableBytes, sizeof (u8));
			cMeshesTable = sMeshesTable + meshesTableBytes[0];

			sUniformsTable = (u8*) calloc (allUniformsTableBytes, sizeof (u8));
			cUniformsTable = sUniformsTable + uniformsTableBytes[0];
			wUniformsTable = cUniformsTable + uniformsTableBytes[1];

			sShadersLoadTable = (u8*) calloc (allShadersLoadTableBytes, sizeof (u8));
			cShadersLoadTable = sShadersLoadTable + shadersLoadTableBytes[0];
			wShadersLoadTable = cShadersLoadTable + shadersLoadTableBytes[1];

			// If not 0 allocate !
			if (sMaterialsCount) sMaterials = new MATERIAL::Material[sMaterialsCount];
			if (cMaterialsCount) cMaterials = new MATERIAL::Material[cMaterialsCount];
			if (wMaterialsCount) wMaterials = new MATERIAL::Material[wMaterialsCount];

		}
		
	}


	void LoadMaterials (
		/* IN  */ Json& json,
		//
		/* OUT */ u8*& sShadersLoadTable,
		/* OUT */ u8*& sUniformsTable,
		/* OUT */ u8*& sMaterialMeshTable,
		/* OUT */ u8& sMaterialsCount,
		/* OUT */ MATERIAL::Material* sMaterials,
		//
		/* OUT */ u8*& cShadersLoadTable,
		/* OUT */ u8*& cUniformsTable,
		/* OUT */ u8*& cMaterialMeshTable,
		/* OUT */ u8& cMaterialsCount,
		/* OUT */ MATERIAL::Material* cMaterials,
		//
		/* OUT */ u8*& wShadersLoadTable,
		/* OUT */ u8*& wUniformsTable,
		/* OUT */ u8*& wMaterialMeshTable,
		/* OUT */ u8& wMaterialsCount,
		/* OUT */ MATERIAL::Material* wMaterials
	) {
		ZoneScopedN("RESOURCES::MATERIALS: LoadMaterials");

		// !!! Moving code to location.hpp !!! 
		ReadMaterialsGroup (GROUP_KEY_SCREEN, json, sShadersLoadTable, sUniformsTable, sMaterialMeshTable, sMaterialsCount, sMaterials);
		ReadMaterialsGroup (GROUP_KEY_CANVAS, json, cShadersLoadTable, cUniformsTable, cMaterialMeshTable, cMaterialsCount, cMaterials);
		ReadMaterialsGroup (GROUP_KEY_WORLD, json, wShadersLoadTable, wUniformsTable, wMaterialMeshTable, wMaterialsCount, wMaterials);
	}

	// 0 - materials
	// 1 - meshes
	// 2 - mesh_id
	// 3 - meshes
	// 4 - mesh_id
	// 0, 1, 2, 3, 4
	// Creates Links Material -> Mesh/es 

}