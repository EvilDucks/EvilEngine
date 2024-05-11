#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"

#include "render/material.hpp"
#include "render/shader.hpp"

// Material is not a GameObject Component!
//  However Meshes which are Components are being referenced here.
//  To change a Material for a said Mesh you need to remove it from
//  one material (which will stop renderring said mesh) and readd it
//  to the material of choice.

namespace MATERIAL {

	struct Material {
		SHADER::Shader program { 0 }; // ! in future it will be a index to shader array structure.
		GLuint texture { 0 };
	};

}

namespace MATERIAL::MESHTABLE {

	// If we ever make any changes to the MESHTABLE structure
	//  it will be easier to change how we read from it in one place
	//  like here instead of going trought many different scripts.
	
	// mat_s, mat1[mesh_s, m1, m2, ..., mn], mat2[mesh_s, m1, m2, ..., mn], ...
	// - We offset by 1 to skip "mat_s" byte.
	// - Offset by prevMaterialMeshes because each material has different amount of meshes.
	// - Finally by current mesh in material.

	// BufforSized count
	// BufforCaped capacity, count

	u8 prevMaterialMeshes = 0;

	void SetRead (const u8& value) {
		prevMaterialMeshes = value;
	}

	void AddRead (const u8& value) {
		prevMaterialMeshes += value;
	}

	auto GetMeshCount (
		u8* materialMeshTable, 
		const u8& materialIndex
	) {
		return materialMeshTable + 1 + prevMaterialMeshes + materialIndex;
	}

	auto GetMesh (
		u8* materialMeshTable, 
		const u8& materialIndex, 
		const u8& meshIndex
	) {
		return materialMeshTable + 2 + prevMaterialMeshes + materialIndex + (meshIndex * 2) + 0;
	}

	auto GetMeshInstancesCount (
		u8* materialMeshTable, 
		const u8& materialIndex, 
		const u8& meshIndex
	) {
		return materialMeshTable + 2 + prevMaterialMeshes + materialIndex + (meshIndex * 2) + 1;
	}


	void GetMaxInstances (
		u8* materialMeshTable,
		u8* instancesCounts
	) {
		auto& tableMaterialsCount = materialMeshTable[0];

		for (u8 iMaterial = 0; iMaterial < tableMaterialsCount; ++iMaterial) {
			const auto& materialMeshesCount = *GetMeshCount (materialMeshTable, iMaterial);

			for (u8 iMesh = 0; iMesh < materialMeshesCount; ++iMesh) {
				const auto& meshId = *GetMesh (materialMeshTable, iMaterial, iMesh);
				const auto& instances = *GetMeshInstancesCount (materialMeshTable, iMaterial, iMesh);
				auto& instancedCount = instancesCounts[meshId];

				// Store Max Instances Per Mesh.
				if (instancedCount < instances) instancedCount = instances;
			}

			AddRead (materialMeshesCount * 2);
		} SetRead (0);
	}

}