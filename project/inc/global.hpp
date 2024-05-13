#pragma once
#include "types.hpp"

#if PLATFORM == PLATFORM_WINDOWS
#include "platform/win/types.hpp"
#else
#include "platform/agn/types.hpp"
#endif

#include "resources/manager.hpp"
#include "resources/materials.hpp"
#include "resources/shaders.hpp"
#include "resources/meshes.hpp"
#include "resources/location.hpp"

#include "scene.hpp"
#include "object.hpp"

#include "render/texture.hpp"


//#include "hid/inputManager.hpp"
#include "player/player.hpp"
#include "components/collisions/collisionsDetection.hpp"
#include "generator/mapGenerator.hpp"

namespace GLOBAL {

	Color4 backgroundColor = Color4 ( 114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f );

	// time of the previous frame
	// time of the 
	double timeSinceLastFrame = 0, timeCurrent = 0, timeDelta = 0;

	WIN::WindowTransform windowTransform { 0, 0, 1200, 640 }; // pos.x, pos.y, size.x, size.y
	//Prepare starting mouse positions
	float lastX = windowTransform[2] / 2.0f;
	float lastY = windowTransform[3] / 2.0f;
	
	INPUT_MANAGER::IM inputManager = nullptr;
	HID_INPUT::Input input = nullptr;
	WIN::Window mainWindow = nullptr;

	int mode = EDITOR::PLAY_MODE;
	int editedObject = 6;

	MAP_GENERATOR::MG mapGenerator = nullptr;

	PLAYER::Player *players = nullptr;
	u64 playerCount = 0;

	// TEST FRUSTUM CULLING
	// --------------------
	/*DEBUG {
		u64 onCPU = 0;
		u64 onGPU = 0;
	};*/

	// --------------------

	glm::vec3 lightPosition = glm::vec3(1.0f, 1.0f, 1.0f);

	// SET DURING INITIALIZATION
	SCENE::SHARED::World sharedWorld;
	SCENE::Scene scene;

	SCENE::Screen screen { 0 };
	SCENE::Canvas canvas { 0 };
	SCENE::Skybox skybox { 0 };
	SCENE::World world   { 0 };

	SCENE::World additionalWorld { 0 };


	void Initialize () {
		ZoneScopedN("GLOBAL: Initialize");

		// Make a debug_directive later...
		// DEBUG GL::GetSpecification ();

		// It's all Data Layer, Memory allocations, Pointer assignments.

		RESOURCES::Json materialsJson;
		RESOURCES::Json meshesJson;
		RESOURCES::Json sceneJson;
		
		{ // SCREEN
			screen.parenthoodsCount = 0; 
			screen.transformsCount = 1; // must be 1! (for root)
		}

		{ // CANVAS
			canvas.parenthoodsCount = 0; 
			canvas.transformsCount = 0;
		}
		
		{ // WORLD
			// Remove them for now. -> Scene Loading 12.05.2024.
			//world.collidersCount[COLLIDER::ColliderGroup::PLAYER]	= 1;
			//world.collidersCount[COLLIDER::ColliderGroup::MAP]	= 1;
			world.collidersCount[COLLIDER::ColliderGroup::PLAYER]	= 0;
			world.collidersCount[COLLIDER::ColliderGroup::MAP]		= 0;
		}

		{ // PLAYERS
			// Remove them for now. -> Scene Loading 12.05.2024.
			//playerCount = 1;
			playerCount = 0;
		}
		

		DEBUG { spdlog::info ("Allocating memory for components and collections."); }

		RESOURCES::MATERIALS::CreateMaterials (
			materialsJson,
			//
			screen.loadTables.shaders, screen.tables.uniforms, screen.tables.meshes, 
			screen.materialsCount, screen.materials,
			//
			canvas.loadTables.shaders, canvas.tables.uniforms, canvas.tables.meshes, 
			canvas.materialsCount, canvas.materials,
			//
			world.loadTables.shaders, world.tables.uniforms, world.tables.meshes, 
			sharedWorld.materialsCount, sharedWorld.materials
		);

		RESOURCES::MESHES::CreateMeshes (
			meshesJson,
			screen.meshesCount, screen.meshes,
			canvas.meshesCount, canvas.meshes,
			sharedWorld.meshesCount, sharedWorld.meshes
		);

		SCENE::SceneLoadContext segmentLoad[2] { 0 };

		{ // Loading main.
			auto& loadHelper = segmentLoad[0];
			auto& cWorld = world;
			//
			const u8 DIFFICULTY = 4; // 0 - 4 (5)
			const u8 EXIT_TYPE = 2;  // 0 - 2 (3)
			// NOW ALWAYS CONSTANT "height": 48
			// NOW ALWAYS CONSTANT "platform_count": 0
			// NOW ALWAYS CONSTANT "trap_count": 0
			//
			RESOURCES::SCENE::Create (
				sceneJson, RESOURCES::MANAGER::SCENES::SEGMENTS[DIFFICULTY + (5 * EXIT_TYPE)],
				//sceneJson, RESOURCES::MANAGER::SCENES::TOWER,
				//sceneJson, RESOURCES::MANAGER::SCENES::ALPHA,
				sharedWorld.materialsCount, sharedWorld.meshesCount, 						// Already set
				cWorld.tables.meshes, cWorld.tables.parenthoodChildren, 					// Tables
				loadHelper.relationsLookUpTable, loadHelper.relationsLookUpTableOffset,		// Helper Logic + what we get
				cWorld.parenthoodsCount, cWorld.transformsCount								// What we actually get.
			);
		}

		//{ // Loading additional.
		//	auto& loadHelper = segmentLoad[1];
		//	auto& cWorld = additionalWorld;
		//	//
		//	const u8 DIFFICULTY = 3; // 0 - 4 (5)
		//	const u8 EXIT_TYPE = 2;  // 0 - 2 (3)
		//	// NOW ALWAYS CONSTANT "height": 48
		//	// NOW ALWAYS CONSTANT "platform_count": 0
		//	// NOW ALWAYS CONSTANT "trap_count": 0
		//	//
		//	RESOURCES::SCENE::Create (
		//		sceneJson, RESOURCES::MANAGER::SCENES::SEGMENTS[DIFFICULTY + (5 * EXIT_TYPE)],
		//		//sceneJson, RESOURCES::MANAGER::SCENES::TOWER,
		//		//sceneJson, RESOURCES::MANAGER::SCENES::ALPHA,
		//		cWorld.materialsCount, cWorld.meshesCount, cWorld.tables.meshes, 			// Already set
		//		cWorld.tables.parenthoodChildren, 											// Helper for now
		//		loadHelper.relationsLookUpTable, loadHelper.relationsLookUpTableOffset,	// Helper Logic + what we get
		//		cWorld.parenthoodsCount, cWorld.transformsCount								// What we actually get.
		//	);
		//}

		{ // SCREEN
			if (screen.parenthoodsCount) screen.parenthoods = new PARENTHOOD::Parenthood[screen.parenthoodsCount] { 0 };
			if (screen.transformsCount) {
				screen.lTransforms = new TRANSFORM::LTransform[screen.transformsCount] { 0 };
				screen.gTransforms = new TRANSFORM::GTransform[screen.transformsCount];
			}
		}

		{ // CANVAS
			if (canvas.parenthoodsCount) canvas.parenthoods = new PARENTHOOD::Parenthood[canvas.parenthoodsCount] { 0 };
			if (canvas.transformsCount) {
				canvas.lTransforms = new TRANSFORM::LTransform[canvas.transformsCount] { 0 };
				canvas.gTransforms = new TRANSFORM::GTransform[canvas.transformsCount];
			}
		}

		{ // WORLD
			if (world.parenthoodsCount) world.parenthoods = new PARENTHOOD::Parenthood[world.parenthoodsCount] { 0 };

			if (world.transformsCount) {
				world.lTransforms = new TRANSFORM::LTransform[world.transformsCount] { 0 };
				world.gTransforms = new TRANSFORM::GTransform[world.transformsCount];
			}

			if (world.collidersCount[COLLIDER::ColliderGroup::PLAYER]) world.colliders[COLLIDER::ColliderGroup::PLAYER] = new COLLIDER::Collider[world.collidersCount[COLLIDER::ColliderGroup::PLAYER]] { 0 };
			if (world.collidersCount[COLLIDER::ColliderGroup::MAP]) world.colliders[COLLIDER::ColliderGroup::MAP] = new COLLIDER::Collider[world.collidersCount[COLLIDER::ColliderGroup::MAP]] { 0 };
		}

		{ // PLAYER
			if (playerCount) players = new PLAYER::Player[playerCount] { 0 };
		}

		DEBUG { spdlog::info ("Creating scene : parenthoods, transforms, meshTable."); }

		{ // Screen

			{ // ROOT
				auto& componentTransform = screen.lTransforms[0];
				auto& local = componentTransform.local;
				componentTransform.id = OBJECT::_06;
				//
				local.position	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}

		}

		//DEBUG spdlog::info ("mc: {0}", world.materialsCount);

		

		{
			world.tables.meshes[0] = sharedWorld.materialsCount;
			auto& loadHelper = segmentLoad[0];

			RESOURCES::SCENE::Load (
				sceneJson, 
				sharedWorld.materialsCount, sharedWorld.meshesCount, 
				world.tables.meshes, world.tables.parenthoodChildren, 
				loadHelper.relationsLookUpTable, loadHelper.relationsLookUpTableOffset,
				world.parenthoodsCount, world.parenthoods, 
				world.transformsCount, world.lTransforms
			);
		}

		DEBUG { spdlog::info ("Precalculating transfroms global position."); }

		//DEBUG {
		//	auto&& c1 = world.parenthoods[0].base.children;
		//	auto&  p1 = world.parenthoods[0].id;
		//	spdlog::info ("{0}: {1}, {2}, {3}, {4}, {5}", p1, c1[0], c1[1], c1[2], c1[3], c1[4]);
		//	//exit (1);
		//	auto&& c1 = world.tables.meshes;
		//	spdlog::info (
		//		"{0}: {1}, ({2}, {3}), ({4}, {5}), {6}, ({7}, {8})", 
		//		c1[0], c1[1], c1[2], c1[3], c1[4], c1[5], c1[6], c1[7], c1[8]
		//	);
		//}
		

		{ // Precalculate Global Trnasfroms
			TRANSFORM::Precalculate (
					world.parenthoodsCount, world.parenthoods,
					world.transformsCount, world.lTransforms, world.gTransforms
			);
			TRANSFORM::Precalculate (
					screen.parenthoodsCount, screen.parenthoods,
					screen.transformsCount, screen.lTransforms, screen.gTransforms
			);
		}

		DEBUG { spdlog::info ("Creating fonts."); }

		{
			auto& VAO = FONT::faceVAO;
			auto& VBO = FONT::faceVBO;
			//
			glGenVertexArrays (1, &VAO);
			glGenBuffers (1, &VBO);
			glBindVertexArray (VAO);
			glBindBuffer (GL_ARRAY_BUFFER, VBO);
			glBufferData (GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray (0);
			glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
			glBindBuffer (GL_ARRAY_BUFFER, 0);
			glBindVertexArray (0);   
		}

		DEBUG { spdlog::info ("Creating textures."); }

		{ // TEXTURE
			const TEXTURE::Atlas dustsAtlas	   { 6, 6, 1, 16, 16 }; // elements, cols, rows, tile_pixels_x, tile_pixels_y
			const TEXTURE::Atlas writtingAtlas { 6, 5, 2, 64, 64 };

			// SCREEN
			auto& texture0 = screen.materials[0].texture;
			auto& texture1 = screen.materials[1].texture;
			auto& texture2 = screen.materials[2].texture;
			// WORLD
			auto& textureW0 = sharedWorld.materials[3].texture;
			
			// Don't overuse memory allocations.
			TEXTURE::HolderCube textureCubeHolder;
			TEXTURE::Holder& textureHolder = textureCubeHolder[0];

			{ // SKYBOX
				for (u8 i = 0; i < TEXTURE::CUBE_FACES_COUNT; ++i) {
					TEXTURE::Load (textureCubeHolder[i], RESOURCES::MANAGER::SKYBOX_NIGHT[i]);
					//TEXTURE::Load (textureCubeHolder[i], RESOURCES::MANAGER::SKYBOX_DEFAULT[i]);
				}
				TEXTURE::CUBEMAP::Create (skybox.texture, textureCubeHolder, TEXTURE::PROPERTIES::defaultRGB);
			}

			stbi_set_flip_vertically_on_load (true);

			TEXTURE::Load (textureHolder, RESOURCES::MANAGER::TEXTURE_BRICK);
			TEXTURE::SINGLE::Create (texture0, textureHolder, TEXTURE::PROPERTIES::defaultRGB);

			TEXTURE::Load (textureHolder, RESOURCES::MANAGER::TEXTURE_TIN_SHEARS);
			TEXTURE::SINGLE::Create (texture1, textureHolder, TEXTURE::PROPERTIES::defaultRGB);

			TEXTURE::Load (textureHolder, RESOURCES::MANAGER::ANIMATED_TEXTURE_2);
			TEXTURE::ARRAY::Create (texture2, textureHolder, TEXTURE::PROPERTIES::alphaPixelNoMipmap, writtingAtlas);
			
			textureW0 = texture0;
		}

		DEBUG { spdlog::info ("Creating materials."); }

		RESOURCES::MATERIALS::LoadMaterials (
			materialsJson,
			screen.loadTables.shaders, screen.tables.uniforms, screen.tables.meshes, 
			screen.materialsCount, screen.materials,
			//
			canvas.loadTables.shaders, canvas.tables.uniforms, canvas.tables.meshes, 
			canvas.materialsCount, canvas.materials,
			//
			world.loadTables.shaders, world.tables.uniforms, world.tables.meshes, 
			sharedWorld.materialsCount, sharedWorld.materials
		);

		DEBUG { spdlog::info ("Creating shader programs."); }

		RESOURCES::SHADERS::Load ( 19, D_SHADERS_SCREEN, screen.loadTables.shaders, screen.tables.uniforms, screen.materials );
		//DEBUG_RENDER GL::GetError (1234);
		//RESOURCES::SHADERS::LoadShaders ( 19, D_SHADERS_CANVAS, canvas.loadTables.shaders, canvas.tables.uniforms, canvas.materials );
		RESOURCES::SHADERS::LoadCanvas (canvas.tables.uniforms, canvas.materials);
		//DEBUG_RENDER GL::GetError (1235);
		RESOURCES::SHADERS::Load ( 18, D_SHADERS_WORLD, world.loadTables.shaders, world.tables.uniforms, sharedWorld.materials );
		//DEBUG_RENDER GL::GetError (1236);
		RESOURCES::SHADERS::LoadSkybox (skybox.shader);

		DEBUG { spdlog::info ("Creating meshes."); }

		u8* sInstancesCounts = (u8*) calloc (screen.meshesCount, sizeof (u8) );
		u8* cInstancesCounts = (u8*) calloc (canvas.meshesCount, sizeof (u8) );
		u8* wInstancesCounts = (u8*) calloc (sharedWorld.meshesCount, sizeof (u8) );

		MATERIAL::MESHTABLE::GetMaxInstances (screen.tables.meshes, sInstancesCounts);
		MATERIAL::MESHTABLE::GetMaxInstances (canvas.tables.meshes, cInstancesCounts);
		MATERIAL::MESHTABLE::GetMaxInstances (world.tables.meshes, wInstancesCounts);

		RESOURCES::MESHES::LoadMeshes (
			meshesJson,
			screen.meshesCount, screen.meshes, sInstancesCounts,
			canvas.meshesCount, canvas.meshes, cInstancesCounts,
			sharedWorld.meshesCount, sharedWorld.meshes, wInstancesCounts,
			skybox.mesh
		);

		free (sInstancesCounts);
		free (cInstancesCounts);
		free (wInstancesCounts);

		DEBUG { spdlog::info ("Creating camera components."); }

		{ // World
			{
				glm::vec3 position = glm::vec3(0.0f, 0.0f, -8.0f);
				// set z to its negative value, if we don't do it camera position on z is its negative value
				position.z = - position.z;

				world.camera.local.position = position;
				glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
				world.camera.local.worldUp = up;
				world.camera.local.front = glm::vec3(0.0f, 0.0f, -1.0f);
				world.camera.local.yaw = CAMERA::YAW;
				world.camera.local.pitch = CAMERA::PITCH;
				world.camera.local.zoom = CAMERA::ZOOM;
				world.camera.local.mouseSensitivity = CAMERA::SENSITIVITY;
				world.camera.local.moveSpeed = CAMERA::SPEED;


				updateCameraVectors(world.camera);
			}
		}

		DEBUG { spdlog::info ("Creating collider components."); }

		// HARDCODDED Collision Game Object
		//u16 CGO1 = 4; // OBJECT::_07_player;
		//u16 CGO2 = 6; // OBJECT::_08_testWall;
		//
		//DEBUG {
		//	CGO1 = 3;
		//	CGO2 = 5;
		//}


		// COLLIDERS
		//{ // world colliders
		//	{
		//		auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::PLAYER];
		//		auto& local = componentCollider->local;
		//		local.group = COLLIDER::ColliderGroup::PLAYER;
		//		local.type = COLLIDER::ColliderType::AABB;
		//		componentCollider->id = CGO1;
		//	}
		//	{
		//		auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::MAP];
		//		auto& local = componentCollider->local;
		//		local.group = COLLIDER::ColliderGroup::MAP;
		//		local.type = COLLIDER::ColliderType::AABB;
		//		componentCollider->id = CGO2;
		//	}
		//}

		//{ // colliders initialization
		//	{
		//		u64 meshIndex = OBJECT::ID_DEFAULT;
		//		OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, world.meshesCount, world.meshes, CGO1);
		//		u64 colliderIndex = OBJECT::ID_DEFAULT;
		//		OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::PLAYER], world.colliders[COLLIDER::ColliderGroup::PLAYER], CGO1);
		//		COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex], world.meshes[meshIndex], world.transformsCount, world.lTransforms);
		//	}
		//	{
		//		u64 meshIndex = OBJECT::ID_DEFAULT;
		//		OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, world.meshesCount, world.meshes, CGO2);
		//		u64 colliderIndex = OBJECT::ID_DEFAULT;
		//		OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::MAP], world.colliders[COLLIDER::ColliderGroup::MAP], CGO2);
		//		COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::MAP][colliderIndex], world.meshes[meshIndex], world.transformsCount, world.lTransforms);
		//	}
		//}

		DEBUG { spdlog::info ("Creating player components."); }

		//{ // players
		//	auto& player = players[0];
		//	auto& local = player.local;
		//	player.id = CGO1;
		//	//
		//	local.name = "TEST PLAYER1";
		//	std::vector<InputDevice> controlScheme;
		//	u64 deviceIndex = 0;
		//	INPUT_MANAGER::FindDevice(inputManager, InputSource::KEYBOARD, 0, deviceIndex);
		//	controlScheme.push_back(inputManager->_devices[deviceIndex]);
		//	inputManager->_devices[deviceIndex].PlayerIndex = 0;
		//	local.controlScheme = controlScheme;
		//	u64 transformIndex = 0;
		//	OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount, world.lTransforms, player.id);
		//	local.transform = &(world.lTransforms[transformIndex]);
		//	u64 colliderIndex = 0;
		//	OBJECT::GetComponentFast<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::PLAYER], world.colliders[COLLIDER::ColliderGroup::PLAYER], player.id);
		//	local.collider = &(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex]);
		//}
		

		DEBUG { spdlog::info ("Creating map generator."); }

		mapGenerator = new MAP_GENERATOR::MapGenerator;
		MAP_GENERATOR::LoadModules(mapGenerator, "res/data/scenes/segments");
		MAP_GENERATOR::GenerateLevel(mapGenerator);

        // HERE !!!
        //MODULE::Module moduleTest = mapGenerator->_generatedLevel[0];

		//DEBUG {
		//	auto&& meshes = world.tables.meshes;
		//	spdlog::info (
		//		"materials: {0}",
		//		meshes[0]
		//	);
		//	spdlog::info (
		//		"meshes: {0}, id: {1}, instances: {2}, id: {3},", 
		//		meshes[1], meshes[2], meshes[3], meshes[4]
		//	);
		//	spdlog::info (
		//		"instances: {0}, meshes: {1}, id: {2}, instances: {3}, ", 
		//		meshes[5], meshes[6], meshes[7], meshes[8]
		//	);
		//	spdlog::info (
		//		"meshes: {0}, id: {1}, instances: {2}, meshes: {3}", 
		//		meshes[9], meshes[10], meshes[11], meshes[12]
		//	);	
		//	spdlog::info (
		//		"id: {0}, instances: {1}, huh: {2}",
		//		meshes[13], meshes[14], meshes[15]
		//	);	
		//}

		DEBUG spdlog::info ("Initialization Complete!");

		// DELETETHIS
		//exit (1);

		// Connect Scene to Screen & World structures.
		scene.skybox = &skybox;
		scene.screen = &screen;
		scene.canvas = &canvas;
		scene.world = &world;
	}


	void Destroy () {
		ZoneScopedN("GLOBAL: Destroy");

		DEBUG { spdlog::info ("Destroying parenthood components."); }
		
		delete[] screen.parenthoods;
		//delete[] screen.tables.parenthoodChildren;

		delete[] canvas.parenthoods;
		//delete[] canvas.tables.parenthoodChildren;

		delete[] world.parenthoods;
		delete[] world.tables.parenthoodChildren;

		DEBUG { spdlog::info ("Destroying mesh components."); }

		RESOURCES::MESHES::DeleteMeshes (
			screen.meshesCount, screen.meshes,
			canvas.meshesCount, canvas.meshes,
			sharedWorld.meshesCount, sharedWorld.meshes
		);

		DEBUG { spdlog::info ("Destroying transfrom components."); }

		delete[] screen.lTransforms;
		delete[] canvas.lTransforms;
		delete[] world.lTransforms;
		delete[] screen.gTransforms;
		delete[] canvas.gTransforms;
		delete[] world.gTransforms;

		DEBUG { spdlog::info ("Destroying collider components."); }

		delete[] world.colliders[COLLIDER::ColliderGroup::MAP];
		delete[] world.colliders[COLLIDER::ColliderGroup::PLAYER];

		DEBUG { spdlog::info ("Destroying players."); }

		delete[] players;

		DEBUG { spdlog::info ("Destroying materials."); }

		RESOURCES::MATERIALS::DestoryMaterials (
			screen.tables.uniforms, screen.tables.meshes, screen.materials,
			canvas.tables.uniforms, canvas.tables.meshes, canvas.materials,
			world.tables.uniforms, world.tables.meshes, 
			sharedWorld.materials
		);

		RESOURCES::MATERIALS::DestroyLoadShaders (
			screen.loadTables.shaders, canvas.loadTables.shaders, world.loadTables.shaders
		);

		DEBUG { spdlog::info ("Destroying shader programs."); }

		for (u64 i = 0; i < screen.materialsCount; ++i) {
			auto& material = screen.materials[i];
			SHADER::Destroy (material.program);
		}

		for (u64 i = 0; i < sharedWorld.materialsCount; ++i) {
			auto& material = sharedWorld.materials[i];
			SHADER::Destroy (material.program);
		}

	}


	void Collisions (std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount, PLAYER::Player *players, u64 playerCount)
	{
		ZoneScopedN("GLOBAL: Collisions");

		CheckOBBCollisions(COLLIDER::ColliderGroup::PLAYER, COLLIDER::ColliderGroup::MAP, GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount);

		for (int i = 0; i < playerCount; i++)
		{
			PLAYER::HandlePlayerCollisions(players[i], colliders, collidersCount);
		}
	}

}
