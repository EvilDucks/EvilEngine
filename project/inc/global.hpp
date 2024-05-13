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

	// --------------------

	glm::vec3 lightPosition = glm::vec3(1.0f, 1.0f, 1.0f);

	// SET DURING INITIALIZATION
	SCENE::SHARED::Screen sharedScreen;
	SCENE::SHARED::Canvas sharedCanvas;
	SCENE::SHARED::World sharedWorld;
	SCENE::Scene scene;

	SCENE::Screen screen { 0 };
	SCENE::Canvas canvas { 0 };
	SCENE::Skybox skybox { 0 };
	SCENE::World world   { 0 };

	u8 segmentsCount = 0;
	SCENE::World* segmentsWorld = nullptr;


	void Initialize () {
		ZoneScopedN("GLOBAL: Initialize");

		// Make a debug_directive later...
		// DEBUG GL::GetSpecification ();

		// It's all Data Layer, Memory allocations, Pointer assignments.

		RESOURCES::Json materialsJson;
		RESOURCES::Json meshesJson;
		RESOURCES::Json sceneJson;
		SCENE::SceneLoadContext sceneLoad { 0 };

		RESOURCES::Json* segmentsJson = nullptr;
		SCENE::SceneLoadContext* segmentLoad = nullptr;
		
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

		DEBUG { spdlog::info ("Creating map generator."); }

		{
			MAP_GENERATOR::ParkourDifficulty difficulty {
        		/*rangePosition*/ 0.5f,
        		/*rangeWidth*/    1.0f
    		};

			MAP_GENERATOR::Modifiers modifiers {
				/*levelLength*/ 				5,
				/*stationaryTrapsAmount*/ 		2,
				/*pushingTrapsAmount*/ 			5,
				/*parkourDifficulty*/ 			difficulty,
				/*windingModuleProbability*/	0.5f
			};

			mapGenerator = new MAP_GENERATOR::MapGenerator;
			mapGenerator->modifiers = modifiers;

			MAP_GENERATOR::LoadModules (mapGenerator, RESOURCES::MANAGER::SEGMENTS);
			MAP_GENERATOR::GenerateLevel (mapGenerator);
			
			segmentsCount = mapGenerator->_generatedLevel.size();

			// Memory allocations...
			segmentsJson	= new RESOURCES::Json[segmentsCount];
			segmentLoad		= new SCENE::SceneLoadContext[segmentsCount] { 0 };
			segmentsWorld	= new SCENE::World[segmentsCount] { 0 };
		}

		DEBUG { spdlog::info ("Allocating memory for components and collections."); }

		RESOURCES::MATERIALS::CreateMaterials (
			materialsJson,
			//
			sharedScreen.loadTables.shaders, sharedScreen.tables.uniforms, screen.tables.meshes, 
			sharedScreen.materialsCount, sharedScreen.materials,
			//
			sharedCanvas.loadTables.shaders, sharedCanvas.tables.uniforms, canvas.tables.meshes, 
			sharedCanvas.materialsCount, sharedCanvas.materials,
			//
			sharedWorld.loadTables.shaders, sharedWorld.tables.uniforms,
			sharedWorld.materialsCount, sharedWorld.materials
		);

		RESOURCES::MESHES::CreateMeshes (
			meshesJson,
			sharedScreen.meshesCount, sharedScreen.meshes,
			sharedCanvas.meshesCount, sharedCanvas.meshes,
			sharedWorld.meshesCount, sharedWorld.meshes
		);

		{ // Loading main.
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
				world.tables.meshes, world.tables.parenthoodChildren, 						// Tables
				sceneLoad.relationsLookUpTable, world.transformsOffset,		// Helper Logic + what we get
				world.parenthoodsCount, world.transformsCount								// What we actually get.
			);
		}

		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // Loading additional.
			auto& segment = mapGenerator->_generatedLevel[iSegment];
			auto& fileJson = segmentsJson[iSegment];
			auto& loadHelper = segmentLoad[iSegment];
			auto& cWorld = segmentsWorld[iSegment];
			//
			DEBUG if (segment.parkourDifficulty < 1.0f || segment.parkourDifficulty > 5.0f) {
				spdlog::error ("Segment difficulty ({0}) set to an invalid value!", segment.parkourDifficulty);
				exit (1);
			}

			const u8 DIFFICULTY = (u8)segment.parkourDifficulty - 1; 	// 3; // 0 - 4 (5)
			const u8 EXIT_TYPE = segment.exitSide; 					// 1;  // 0 - 2 (3)

			//DEBUG spdlog::info ("aaa: {0}, {1}", DIFFICULTY, EXIT_TYPE);
			//
			RESOURCES::SCENE::Create (
				fileJson, RESOURCES::MANAGER::SCENES::SEGMENTS[DIFFICULTY + (5 * EXIT_TYPE)],
				sharedWorld.materialsCount, sharedWorld.meshesCount, 					// Already set
				cWorld.tables.meshes, cWorld.tables.parenthoodChildren, 				// Tables
				loadHelper.relationsLookUpTable, cWorld.transformsOffset,	// Helper Logic + what we get
				cWorld.parenthoodsCount, cWorld.transformsCount							// What we actually get.
			);
		}

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

			for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) {
				auto& cWorld = segmentsWorld[iSegment];
				if (cWorld.parenthoodsCount) {
					cWorld.parenthoods = new PARENTHOOD::Parenthood[cWorld.parenthoodsCount] { 0 };
				}
			}

			for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) {
				auto& cWorld = segmentsWorld[iSegment];
				if (cWorld.transformsCount) {
					cWorld.lTransforms = new TRANSFORM::LTransform[cWorld.transformsCount] { 0 };
					cWorld.gTransforms = new TRANSFORM::GTransform[cWorld.transformsCount];
				}
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

		{ // MAIN
			RESOURCES::SCENE::Load (
				sceneJson, 
				sharedWorld.materialsCount, sharedWorld.meshesCount, 
				world.tables.meshes, world.tables.parenthoodChildren, 
				sceneLoad.relationsLookUpTable, world.transformsOffset,
				world.parenthoodsCount, world.parenthoods, 
				world.transformsCount, world.lTransforms
			);
		}

		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // SEGMENTS
			auto& fileJson = segmentsJson[iSegment];
			auto& loadHelper = segmentLoad[iSegment];
			auto& cWorld = segmentsWorld[iSegment];

			//DEBUG { spdlog::info ("load!"); }

			//DEBUG spdlog::info ("aaa: {0}, {1}", cWorld.transformsCount, cWorld.parenthoodsCount);
			RESOURCES::SCENE::Load (
				fileJson, 
				sharedWorld.materialsCount, sharedWorld.meshesCount, 
				cWorld.tables.meshes, cWorld.tables.parenthoodChildren, 
				loadHelper.relationsLookUpTable, cWorld.transformsOffset,
				cWorld.parenthoodsCount, cWorld.parenthoods, 
				cWorld.transformsCount, cWorld.lTransforms
			);
		}

		// free uneeded resources...
		delete[] segmentsJson;
		delete[] segmentLoad;

		DEBUG { spdlog::info ("Precalculating transfroms global position."); }
		
		// To make every segment higher and rotated.
		auto& fSegment = mapGenerator->_generatedLevel[0];
		u8 side = fSegment.exitSide;
		//
		for (u8 iSegment = 1; iSegment < segmentsCount; ++iSegment) { 
			auto& segment = mapGenerator->_generatedLevel[iSegment];
			auto& cWorld = segmentsWorld[iSegment];
			cWorld.lTransforms[0].local.position.y += (24.0f * iSegment);
			cWorld.lTransforms[0].local.rotation.y += (90.0f * side);
			side = (side + segment.exitSide) % 4;
		}

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

		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // Precalculate Global Trnasfroms
			auto& cWorld = segmentsWorld[iSegment];
			TRANSFORM::Precalculate (
					cWorld.parenthoodsCount, cWorld.parenthoods,
					cWorld.transformsCount, cWorld.lTransforms, cWorld.gTransforms
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
			auto& texture0 = sharedScreen.materials[0].texture;
			auto& texture1 = sharedScreen.materials[1].texture;
			auto& texture2 = sharedScreen.materials[2].texture;
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
			sharedScreen.loadTables.shaders, sharedScreen.tables.uniforms, screen.tables.meshes, 
			sharedScreen.materialsCount, sharedScreen.materials,
			//
			sharedCanvas.loadTables.shaders, sharedCanvas.tables.uniforms, canvas.tables.meshes, 
			sharedCanvas.materialsCount, sharedCanvas.materials,
			//
			sharedWorld.loadTables.shaders, sharedWorld.tables.uniforms,
			sharedWorld.materialsCount, sharedWorld.materials
		);

		DEBUG { spdlog::info ("Creating shader programs."); }

		RESOURCES::SHADERS::Load ( 19, D_SHADERS_SCREEN, sharedScreen.loadTables.shaders, sharedScreen.tables.uniforms, sharedScreen.materials );
		//DEBUG_RENDER GL::GetError (1234);
		//RESOURCES::SHADERS::LoadShaders ( 19, D_SHADERS_CANVAS, sharedCanvas.loadTables.shaders, sharedCanvas.tables.uniforms, sharedCanvas.materials );
		RESOURCES::SHADERS::LoadCanvas (sharedCanvas.tables.uniforms, sharedCanvas.materials);
		//DEBUG_RENDER GL::GetError (1235);
		RESOURCES::SHADERS::Load ( 18, D_SHADERS_WORLD, sharedWorld.loadTables.shaders, sharedWorld.tables.uniforms, sharedWorld.materials );
		//DEBUG_RENDER GL::GetError (1236);
		RESOURCES::SHADERS::LoadSkybox (skybox.shader);

		DEBUG { spdlog::info ("Creating meshes."); }

		u8* sInstancesCounts = (u8*) calloc (sharedScreen.meshesCount, sizeof (u8) );
		u8* cInstancesCounts = (u8*) calloc (sharedCanvas.meshesCount, sizeof (u8) );
		u8* wInstancesCounts = (u8*) calloc (sharedWorld.meshesCount, sizeof (u8) );

		{
			MATERIAL::MESHTABLE::GetMaxInstances (screen.tables.meshes, sInstancesCounts);
			MATERIAL::MESHTABLE::GetMaxInstances (canvas.tables.meshes, cInstancesCounts);
			MATERIAL::MESHTABLE::GetMaxInstances (world.tables.meshes, wInstancesCounts);

			for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // Precalculate Global Trnasfroms
				auto& cWorld = segmentsWorld[iSegment];
				MATERIAL::MESHTABLE::GetMaxInstances (cWorld.tables.meshes, wInstancesCounts);
			}
		}

		RESOURCES::MESHES::LoadMeshes (
			meshesJson,
			sharedScreen.meshesCount, sharedScreen.meshes, sInstancesCounts,
			sharedCanvas.meshesCount, sharedCanvas.meshes, cInstancesCounts,
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

		// Connect Scene to Screen & World structures.
		scene.skybox = &skybox;
		scene.screen = &screen;
		scene.canvas = &canvas;
		scene.world = &world;
	}


	void DestroyWorld (SCENE::World& world) {
		DEBUG { spdlog::info ("Destroying parenthood components."); }
		delete[] world.parenthoods;
		delete[] world.tables.parenthoodChildren;
		DEBUG { spdlog::info ("Destroying transfrom components."); }
		delete[] world.lTransforms;
		delete[] world.gTransforms;
		DEBUG { spdlog::info ("Destroying collider components."); }
		delete[] world.colliders[COLLIDER::ColliderGroup::MAP];
		delete[] world.colliders[COLLIDER::ColliderGroup::PLAYER];
		DEBUG { spdlog::info ("Destroying render objects."); }
		delete[] world.tables.meshes;
	}


	void Destroy () {
		ZoneScopedN("GLOBAL: Destroy");

		// !!!! segmentsWorld = new SCENE::World[segmentsCount] { 0 };

		DEBUG { spdlog::info ("Destroying parenthood components."); }
		
		delete[] screen.parenthoods;
		//delete[] screen.tables.parenthoodChildren;

		delete[] canvas.parenthoods;
		//delete[] canvas.tables.parenthoodChildren;

		delete[] world.parenthoods;
		delete[] world.tables.parenthoodChildren;

		DEBUG { spdlog::info ("Destroying mesh components."); }

		RESOURCES::MESHES::DeleteMeshes (
			sharedScreen.meshesCount, sharedScreen.meshes,
			sharedCanvas.meshesCount, sharedCanvas.meshes,
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
			sharedScreen.tables.uniforms, screen.tables.meshes, sharedScreen.materials,
			sharedCanvas.tables.uniforms, canvas.tables.meshes, sharedCanvas.materials,
			sharedWorld.tables.uniforms, sharedWorld.materials
		);

		delete[] world.tables.meshes;

		RESOURCES::MATERIALS::DestroyLoadShaders (
			sharedScreen.loadTables.shaders, sharedCanvas.loadTables.shaders, sharedWorld.loadTables.shaders
		);

		DEBUG { spdlog::info ("Destroying shader programs."); }

		for (u64 i = 0; i < sharedScreen.materialsCount; ++i) {
			auto& material = sharedScreen.materials[i];
			SHADER::Destroy (material.program);
		}

		for (u64 i = 0; i < sharedCanvas.materialsCount; ++i) {
			auto& material = sharedCanvas.materials[i];
			SHADER::Destroy (material.program);
		}

		for (u64 i = 0; i < sharedWorld.materialsCount; ++i) {
			auto& material = sharedWorld.materials[i];
			SHADER::Destroy (material.program);
		}

		DEBUG { spdlog::info ("Destroying other words!"); }

		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // Precalculate Global Trnasfroms
			auto& cWorld = segmentsWorld[iSegment];
			DestroyWorld (cWorld);
		}

		DEBUG { spdlog::info ("Successfully FREED all allocated memory!"); }

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
