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
		SHADER::Shader program { 0 };
		Range<MESH::Mesh*> meshes { 0 };
	};

	// void FindMaterialByMesh()

}