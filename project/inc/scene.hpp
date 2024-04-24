#pragma once

#include <map>
#include "types.hpp"

// Components
#include "components/parenthood.hpp"
#include "components/transform.hpp"
#include "components/camera.hpp"
#include "components/collisions/collider.hpp"
//
#include "util/animation.hpp"
#include "util/boundingFrustum.hpp"
//#include "render/shader.hpp"

// Collections
#include "render/mesh.hpp"
#include "render/material.hpp"

// Tables & LoadTables
#include "util/sizedBuffor.hpp"

namespace SCENE {

	struct RuntimeTables {
		u8* meshes;		// materials_count, material[meshes_count, mesh_id], ...
		u8* uniforms;	// shaders_count, uniforms[uniforms_count, uniform_id], ...
	};

	struct LoadTables {
		u8* shaders;	// shaders_count, shader[vertex_path, fragment_path, uniforms_count, uniform["uniform_name"]], ...
	};

	struct EditorTables {
		u8* shadersNames;
	};

	struct Skybox {
		SHADER::Shader shader;
		MESH::Mesh mesh;
		GLuint texture;
	};

	struct Screen { // -> Snapped to screen
		/* Tables */
		LoadTables loadTables;
		RuntimeTables tables;
		/* Collections */
		u64 materialsCount;
		MATERIAL::Material* materials;
		/* COMPONENTS */
		u64 meshesCount;
		MESH::Mesh* meshes;
		u64 parenthoodsCount;
		PARENTHOOD::Parenthood* parenthoods;
		u64 transformsCount;
		TRANSFORM::Transform* transforms;
	};

	struct Canvas { // -> Orto projection and camera and model without z-axis
		/* Tables */
		LoadTables loadTables;
		RuntimeTables tables;
		/* Collections */
		u64 materialsCount;
		MATERIAL::Material* materials;
		/* COMPONENTS */
		u64 meshesCount;
		MESH::Mesh* meshes;
		u64 parenthoodsCount;
		PARENTHOOD::Parenthood* parenthoods;
		u64 transformsCount;
		TRANSFORM::Transform* transforms;
        std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders {};
        std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount {};
	};

	struct World {
		/* Tables */
		LoadTables loadTables;
		RuntimeTables tables;
		/* Collections */
		u64 materialsCount;
		MATERIAL::Material* materials;
		/* COMPONENTS */
		u64 meshesCount;
		MESH::Mesh* meshes;
		u64 parenthoodsCount;
		PARENTHOOD::Parenthood* parenthoods;
		u64 transformsCount;
		TRANSFORM::Transform* transforms;
        CAMERA::Camera camera;
        BOUNDINGFRUSTUM::Frustum camFrustum;
        std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders {};
        std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount {};
	};

	struct Scene {
		Skybox* skybox = nullptr;
		Screen* screen = nullptr;
		Canvas* canvas = nullptr;
		World* world = nullptr;
	};

}