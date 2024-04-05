#pragma once
#include "types.hpp"

// Components
#include "components/parenthood.hpp"
#include "components/transform.hpp"
#include "components/camera.hpp"

// Collections
#include "render/mesh.hpp"
#include "render/material.hpp"

namespace SCENE {

	struct Screen { // -> Snapped to screen
		/* OTHER */
		MATERIAL::MaterialMeshTable* materialMeshTable = nullptr;
		u64 materialsCount = 0;
		MATERIAL::Material* materials = nullptr;
		/* COMPONENTS */
		u64 meshesCount = 0;
		MESH::Mesh* meshes = nullptr;
		u64 parenthoodsCount = 0;
		PARENTHOOD::Parenthood* parenthoods = nullptr;
		u64 transformsCount = 0;
		TRANSFORM::Transform* transforms = nullptr;
	};

	struct Canvas { // -> Orto projection and camera and model without z-axis
		/* OTHER */
		MATERIAL::MaterialMeshTable* materialMeshTable = nullptr;
		u64 materialsCount = 0;
		MATERIAL::Material* materials = nullptr;
		/* COMPONENTS */
		u64 meshesCount = 0;
		MESH::Mesh* meshes = nullptr;
		u64 parenthoodsCount = 0;
		PARENTHOOD::Parenthood* parenthoods = nullptr;
		u64 transformsCount = 0;
		TRANSFORM::Transform* transforms = nullptr;
	};

	struct World {
		/* OTHER */
		MATERIAL::MaterialMeshTable* materialMeshTable = nullptr;
		u64 materialsCount = 0;
		MATERIAL::Material* materials = nullptr;
		/* COMPONENTS */
		u64 meshesCount = 0;
		MESH::Mesh* meshes = nullptr;
		u64 parenthoodsCount = 0;
		PARENTHOOD::Parenthood* parenthoods = nullptr;
		u64 transformsCount = 0;
		TRANSFORM::Transform* transforms = nullptr;
        CAMERA::Camera camera;
	};

	struct Scene {
		Screen* screen = nullptr;
		Canvas* canvas = nullptr;
		World* world = nullptr;
	};

}