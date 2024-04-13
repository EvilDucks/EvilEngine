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

	// Material's ID it's it's index in the array of materials!
	// MeshTableIndex it's Material's ID!

	using MaterialMeshTable = u8;

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

	u8 prevMaterialMeshes = 0;

	void SetRead (const u8& value) {
		prevMaterialMeshes = value;
	}

	void AddRead (const u8& value) {
		prevMaterialMeshes += value;
	}

	auto GetMeshCount (
		MaterialMeshTable* materialMeshTable, 
		const u8& materialIndex
	) {
		return materialMeshTable + 1 + prevMaterialMeshes + materialIndex;
	}

	auto GetMesh (
		MaterialMeshTable* materialMeshTable, 
		const u8& materialIndex, 
		const u8& meshIndex
	) {
		return materialMeshTable + 2 + prevMaterialMeshes + materialIndex + meshIndex;
	}

}