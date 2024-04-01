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

	// a
	// an array of 256bit values each representing an index in material array.
	// MeshTable meshTableMat1 { 0, 1, 2, 3, 4, 5 };

	// b
	// an array of 2-256bit values representing index and length in material array.
	// MeshTable meshTableMat1 { {0, 1}, {2, 3} };

	struct MeshRange {
		u8 meshIndex;
		u8 meshRange;
	};

	//struct MeshTable {
	//	u64 rangesCount = 0;
	//	MeshRange* ranges = nullptr;
	//}

	struct Material {
		SHADER::Shader program { 0 };
		Range<MESH::Mesh*> meshes { 0 };
		//MeshTableIndex meshTableIndex = 0;
	};

	// void FindMaterialByMesh()

}