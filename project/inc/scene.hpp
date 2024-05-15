#pragma once

#include <map>
#include "types.hpp"

// Components
#include "components/parenthood.hpp"
#include "components/transform.hpp"
#include "components/camera.hpp"
#include "components/collisions/collider.hpp"
#include "components/ui/uiManager.hpp"
//
#include "util/animation.hpp"
#include "util/boundingFrustum.hpp"
//#include "render/shader.hpp"

// Collections
#include "render/mesh.hpp"
#include "render/material.hpp"

// Tables & LoadTables
#include "util/sizedBuffor.hpp"


namespace SCENE::SHARED {

	struct LoadTables {
		u8* shaders;	// shaders_count, shader[vertex_path, fragment_path, uniforms_count, uniform["uniform_name"]], ...
	};

	struct RuntimeTables {
		u8* uniforms;	// shaders_count, uniforms[uniforms_count, uniform_id], ...
	};

	struct Screen {
		LoadTables loadTables;
		RuntimeTables tables;
		//
		u8 materialsCount;
		MATERIAL::Material* materials;
		u8 meshesCount;
		MESH::Mesh* meshes;
	};

	struct Canvas {
		LoadTables loadTables;
		RuntimeTables tables;
		//
		u8 materialsCount;
		MATERIAL::Material* materials;
		u8 meshesCount;
		MESH::Mesh* meshes;
	};

	struct World {
		LoadTables loadTables;
		RuntimeTables tables;
		//
		u8 materialsCount;
		MATERIAL::Material* materials;
		u8 meshesCount;
		MESH::Mesh* meshes;
	};

}

namespace SCENE {

	struct RuntimeTables {
		u8* meshes;					// materials_count, material[meshes_count, mesh_id], ...
		u16* parenthoodChildren;
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
		RuntimeTables tables;
		/* Collections */
		/* COMPONENTS */
		u16 parenthoodsCount;
		u16 transformsOffset;
		PARENTHOOD::Parenthood* parenthoods;
		u16 transformsCount;
		TRANSFORM::LTransform* lTransforms;
		TRANSFORM::GTransform* gTransforms;
	};

	struct Canvas { // -> Orto projection and camera and model without z-axis
		/* Tables */
		RuntimeTables tables;
		/* Collections */
		/* COMPONENTS */
		u16 parenthoodsCount;
		PARENTHOOD::Parenthood* parenthoods;
		u16 transformsCount;
		u16 transformsOffset;
		TRANSFORM::LTransform* lTransforms;
		TRANSFORM::GTransform* gTransforms;
        std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders {};
        std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount {};
        UI::BUTTON::Button* buttons;
        u16 buttonsCount;
	};

	struct World {
		/* Tables */
		RuntimeTables tables;
		/* COMPONENTS */
		u16 parenthoodsCount;
		PARENTHOOD::Parenthood* parenthoods;
		u16 transformsCount;
		u16 transformsOffset;
		TRANSFORM::LTransform* lTransforms;
		TRANSFORM::GTransform* gTransforms;
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

	// Helper struct for scene loading procedure.
	struct SceneLoadContext {
		// Helper array for sorting TRANSFROM's.
		u16* relationsLookUpTable;
		//u8 relationsLookUpTableOffset;
	};

}