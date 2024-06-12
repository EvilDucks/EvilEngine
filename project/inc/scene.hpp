#pragma once

#include <map>
#include "types.hpp"

// Components
#include "components/parenthood.hpp"
#include "components/transform.hpp"
#include "components/camera.hpp"
#include "components/rotating.hpp"
#include "components/collisions/collider.hpp"
#include "components/ui/button.hpp"
#include "components/ui/rect.hpp"
// Weird convension components...
#include "player/player.hpp"
#include "resources/model.hpp"

//
#include "util/animation.hpp"
#include "util/boundingFrustum.hpp"

// Collections
#include "render/mesh.hpp"
#include "render/material.hpp"

// Tables & LoadTables
#include "util/sizedBuffor.hpp"
#include "components/rigidbody.hpp"

//Checkpoint Manager
#include "components/checkpoints/checkpoint.hpp"


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


	struct Enviroment {
		Skybox skybox;
	};

	struct Screen { // -> Snapped to screen
		/* Tables */
		RuntimeTables tables;
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
		/* COMPONENTS */
		u16 parenthoodsCount;
		PARENTHOOD::Parenthood* parenthoods;
		u16 rectanglesCount;
		RECTANGLE::LRectangle* lRectangles;
		RECTANGLE::GRectangle* gRectangles;
		UI::BUTTON::Button* buttons;
        u16 buttonsCount;
		//
        std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders {};
        std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount {};
        
	};

	struct World {
		/* Tables */
		RuntimeTables tables;
		/* COMPONENTS */
		u16 parenthoodsCount;
		PARENTHOOD::Parenthood* parenthoods;
		//
		u16 transformsCount;
		u16 transformsOffset;
		TRANSFORM::LTransform* lTransforms;
		TRANSFORM::GTransform* gTransforms;
		//
		u16 rotatingsCount;
		ROTATING::Rotating* rotatings;
		//
        std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders {};
        std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount {};
		//
        u16 rigidbodiesCount;
        RIGIDBODY::Rigidbody* rigidbodies;
		//
		u16 playersCount;
		PLAYER::Player* players;
		//
        u8 modelsCount;
        MODEL::Model* models;

        CHECKPOINT::Checkpoint* checkpoints;
	};

	//struct Scene {
	//	Skybox* skybox;
	//	Screen* screen;
	//	Canvas* canvas;
	//	World* world;
	//};

	struct Scene {
		Skybox* skybox = nullptr;
		Screen* screen = nullptr;
		Canvas* canvas = nullptr;
		World* world = nullptr;
	};

	// A Packed scene is a structure that contains the 
	//  information of all that is rendered in a game world.
	//struct PackedScenes {
	//	Skybox* skybox; // Enviroment struct
	//	// 
	//	u8 screensCount;
	//	u8 canvasesCount;
	//	u8 worldsCount;
	//	// 
	//	Screen** screens;
	//	Canvas** canvases;
	//	World** worlds;
	//	// 
	//	SHARED::Screen* sharedScreen;
	//	SHARED::Canvas* sharedCanvas;
	//	SHARED::World* sharedWorld;
	//};

	// Helper struct for scene loading procedure.
	struct SceneLoadContext {
		// Helper array for sorting TRANSFROM's.
		u16* relationsLookUpTable;
	};


	void CreateWorld (World& world) {
		if (world.parenthoodsCount) world.parenthoods = new PARENTHOOD::Parenthood[world.parenthoodsCount] { 0 };

		if (world.transformsCount) {
			world.lTransforms = new TRANSFORM::LTransform[world.transformsCount] { 0 };
			world.gTransforms = new TRANSFORM::GTransform[world.transformsCount];
		}

		if (world.collidersCount[COLLIDER::ColliderGroup::PLAYER]) 
			world.colliders[COLLIDER::ColliderGroup::PLAYER] = new COLLIDER::Collider[world.collidersCount[COLLIDER::ColliderGroup::PLAYER]] { 0 };
		if (world.collidersCount[COLLIDER::ColliderGroup::MAP]) 
			world.colliders[COLLIDER::ColliderGroup::MAP] = new COLLIDER::Collider[world.collidersCount[COLLIDER::ColliderGroup::MAP]] { 0 };
        if (world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]) 
			world.colliders[COLLIDER::ColliderGroup::TRIGGER] = new COLLIDER::Collider[world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]] { 0 };

        if (world.rigidbodiesCount) 
			world.rigidbodies = new RIGIDBODY::Rigidbody[world.rigidbodiesCount] { 0 };
		if (world.rotatingsCount)
			world.rotatings = new ROTATING::Rotating[world.rotatingsCount] { 0 };
	}

}