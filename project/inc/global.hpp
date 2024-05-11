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

#include "render/systems.hpp"
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
	SCENE::Scene scene;

	SCENE::Screen screen { 0 };
	SCENE::Canvas canvas { 0 };
	SCENE::Skybox skybox { 0 };
	SCENE::World world   { 0 };


	void GetMaxInstances (
		u8* materialMeshTable,
		u8* instancesCounts
	) {
		auto& tableMaterialsCount = materialMeshTable[0];

		for (u8 iMaterial = 0; iMaterial < tableMaterialsCount; ++iMaterial) {
			const auto& materialMeshesCount = *MATERIAL::MESHTABLE::GetMeshCount (materialMeshTable, iMaterial);

			for (u8 iMesh = 0; iMesh < materialMeshesCount; ++iMesh) {
				const auto& meshId = *MATERIAL::MESHTABLE::GetMesh (materialMeshTable, iMaterial, iMesh);
				const auto& instances = *MATERIAL::MESHTABLE::GetMeshInstancesCount (materialMeshTable, iMaterial, iMesh);
				auto& instancedCount = instancesCounts[meshId];

				// Store Max Instances Per Mesh.
				if (instancedCount < instances) instancedCount = instances;
			}

			MATERIAL::MESHTABLE::AddRead (materialMeshesCount * 2);
		} MATERIAL::MESHTABLE::SetRead (0);
	}

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
			//world.parenthoodsCount = 2;
			//world.transformsCount = 7; // must be 1! (for root)
            world.collidersCount[COLLIDER::ColliderGroup::PLAYER] = 1;
            world.collidersCount[COLLIDER::ColliderGroup::MAP] = 1;
		}
		playerCount = 1;

		DEBUG { spdlog::info ("Allocating memory for components and collections."); }

        world.modelsCount = 1;
        world.models = new MODEL::Model[world.modelsCount]{ 0 };

        RESOURCES::MANAGER::LoadModels(world.modelsCount, world.models);

		RESOURCES::MATERIALS::CreateMaterials (
			materialsJson,
			screen.loadTables.shaders, screen.tables.uniforms, screen.tables.meshes, screen.materialsCount, screen.materials,
			canvas.loadTables.shaders, canvas.tables.uniforms, canvas.tables.meshes, canvas.materialsCount, canvas.materials,
			world.loadTables.shaders, world.tables.uniforms, world.tables.meshes, world.materialsCount, world.materials
		);

		RESOURCES::MESHES::CreateMeshes (
			meshesJson,
			screen.meshesCount, screen.meshes,
			canvas.meshesCount, canvas.meshes,
			world.meshesCount, world.meshes
		);

		// Helper array to for sorting TRANSFROM's.
		u16* wRelationsLookUpTable = nullptr;

		RESOURCES::SCENE::Create (
			sceneJson, 
			world.materialsCount, world.meshesCount, world.tables.meshes, wRelationsLookUpTable,
			world.parenthoodsCount, world.transformsCount
		);

		//DEBUG spdlog::info (
		//	"P: {0}, T: {1}", 
		//	world.parenthoodsCount, world.transformsCount
		//);

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

		DEBUG { spdlog::info ("Creating parenthood relations."); }

		{ // Create parenthood relation ( Using NEW )

			// 1 example
			//1 assert(world.parenthoodsCount == 1);
			//1 {
			//1 	auto& compomnentParenthood = world.parenthoods[0];
			//1 	auto& parenthood = compomnentParenthood.base;
			//1 	parenthood.childrenCount = 2;
			//1 	parenthood.children = new GameObjectID[parenthood.childrenCount] {
			//1 		OBJECT::_4, OBJECT::_5
			//1 	};
			//1 	compomnentParenthood.id = OBJECT::_3;
			//1 }


			// root
			// -> kostka
			//   -> plane
			
			// 2 example
			assert(world.parenthoodsCount == 2);
			{  
				// ! ORDER OF CHILDREN IS IMPORTANT WHEN USING "GetComponentFast" !
				//  meaning if OBJECT::_A is later in TRANSFORMS then OBJECT::_B
				//  then OBJECT_B should be first on the list and later OBJECT::_A.
				auto& componentParenthood = world.parenthoods[0];
				auto& parenthood = componentParenthood.base;
                componentParenthood.id = OBJECT::_03;
				parenthood.childrenCount = 5;
				parenthood.children = new GameObjectID[parenthood.childrenCount] {
					OBJECT::_04, OBJECT::_07_player, OBJECT::_13_LIGHT_1, OBJECT::_08_testWall, OBJECT::_12_GROUND
				};
			}
			{
				auto& componentParenthood = world.parenthoods[1];
				auto& parenthood = componentParenthood.base;
                componentParenthood.id = OBJECT::_04;
				parenthood.childrenCount = 1;
				parenthood.children = new GameObjectID[parenthood.childrenCount] {
					OBJECT::_05
				};
			}
            //{
            //    auto& compomnentParenthood = world.parenthoods[2];
            //    auto& parenthood = compomnentParenthood.base;
            //    compomnentParenthood.id = OBJECT::_player;
            //    parenthood.childrenCount = 0;
			//
            //}
		}

		DEBUG { spdlog::info ("Creating transfrom components."); }

		{ // World
			{ // ROOT
				auto& componentTransform = world.lTransforms[0];
				auto& local = componentTransform.local;
				componentTransform.id = OBJECT::_03;
				//
				local.position	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}
			{ // ROT CUBE
				auto& componentTransform = world.lTransforms[1];
				auto& local = componentTransform.local;
				componentTransform.id = OBJECT::_04;
				//
				local.position	= glm::vec3 (-1.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 15.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}
            { // PLAYER
                auto& componentTransform = world.lTransforms[2];
                auto& local = componentTransform.local;
                componentTransform.id = OBJECT::_07_player;
                //
                local.position	= glm::vec3 (0.0f, 0.0f, 2.0f);
                local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
                local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
            }
			{ // LIGHT
                auto& componentTransform = world.lTransforms[3];
                auto& local = componentTransform.local;
                componentTransform.id = OBJECT::_13_LIGHT_1;
                //
                local.position	= glm::vec3 (4.0f, 0.0f, -4.0f);
                local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
                local.scale		= glm::vec3 (0.5f, 0.5f, 0.5f);
            }
			{ // WALL
                auto& componentTransform = world.lTransforms[4];
                auto& local = componentTransform.local;
                componentTransform.id = OBJECT::_08_testWall;
                //
                local.position	= glm::vec3 (0.0f, 0.0f, -10.0f);
                local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
                local.scale		= glm::vec3 (5.0f, 3.0f, 0.5f);
            }
			{ // ROT PLANE
				auto& componentTransform = world.lTransforms[5];
				auto& local = componentTransform.local;
				componentTransform.id = OBJECT::_05;
				//
				local.position	= glm::vec3 (2.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}
			{ // GROUND PLANE
				auto& componentTransform = world.lTransforms[6];
				auto& local = componentTransform.local;
				componentTransform.id = OBJECT::_12_GROUND;
				//
				local.position	= glm::vec3 (0.0f, -2.0f, 0.0f);
				local.rotation	= glm::vec3 (90.0f, 0.0f, 0.0f);
				local.scale		= glm::vec3 (20.0f, 20.0f, 20.0f);
			}
		}

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

		RESOURCES::SCENE::Load (
			sceneJson, 
			world.materialsCount, world.meshesCount, world.tables.meshes, wRelationsLookUpTable,
			world.parenthoodsCount, world.parenthoods, 
			world.transformsCount, world.lTransforms
		);

		DEBUG { spdlog::info ("Precalculating transfroms global position."); }

		{ // Precalculate Global Trnasfroms
            RENDER::SYSTEMS::PrecalculateGlobalTransforms(
                    world.parenthoodsCount, world.parenthoods,
                    world.transformsCount, world.lTransforms, world.gTransforms
            );
			//
            RENDER::SYSTEMS::PrecalculateGlobalTransforms(
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
			auto& textureW0 = world.materials[3].texture;
			
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
			screen.loadTables.shaders, screen.tables.uniforms, screen.tables.meshes, screen.materialsCount, screen.materials,
			canvas.loadTables.shaders, canvas.tables.uniforms, canvas.tables.meshes, canvas.materialsCount, canvas.materials,
			world.loadTables.shaders, world.tables.uniforms, world.tables.meshes, world.materialsCount, world.materials
		);

		DEBUG { spdlog::info ("Creating shader programs."); }

		RESOURCES::SHADERS::Load ( 19, D_SHADERS_SCREEN, screen.loadTables.shaders, screen.tables.uniforms, screen.materials );
		//DEBUG_RENDER GL::GetError (1234);
		//RESOURCES::SHADERS::LoadShaders ( 19, D_SHADERS_CANVAS, canvas.loadTables.shaders, canvas.tables.uniforms, canvas.materials );
		RESOURCES::SHADERS::LoadCanvas (canvas.tables.uniforms, canvas.materials);
		//DEBUG_RENDER GL::GetError (1235);
		RESOURCES::SHADERS::Load ( 18, D_SHADERS_WORLD, world.loadTables.shaders, world.tables.uniforms, world.materials );
		//DEBUG_RENDER GL::GetError (1236);
		RESOURCES::SHADERS::LoadSkybox (skybox.shader);

		DEBUG { spdlog::info ("Creating meshes."); }

		u8* sInstancesCounts = (u8*) calloc (screen.meshesCount, sizeof (u8) );
		u8* cInstancesCounts = (u8*) calloc (canvas.meshesCount, sizeof (u8) );
		u8* wInstancesCounts = (u8*) calloc (world.meshesCount, sizeof (u8) );

		GetMaxInstances (screen.tables.meshes, sInstancesCounts);
		GetMaxInstances (canvas.tables.meshes, cInstancesCounts);
		GetMaxInstances (world.tables.meshes, wInstancesCounts);

		RESOURCES::MESHES::LoadMeshes (
			meshesJson,
			screen.meshesCount, screen.meshes, sInstancesCounts,
			canvas.meshesCount, canvas.meshes, cInstancesCounts,
			world.meshesCount, world.meshes, wInstancesCounts,
			skybox.mesh
		);

		free (sInstancesCounts);
		free (cInstancesCounts);
		free (wInstancesCounts);

        DEBUG { spdlog::info ("Creating camera component."); }

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

        // COLLIDERS
        {// world colliders
            {
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::PLAYER];
                auto& local = componentCollider->local;
                local.group = COLLIDER::ColliderGroup::PLAYER;
                local.type = COLLIDER::ColliderType::AABB;
                componentCollider->id = OBJECT::_07_player;
            }

            {
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::MAP];
                auto& local = componentCollider->local;
                local.group = COLLIDER::ColliderGroup::MAP;
                local.type = COLLIDER::ColliderType::AABB;
                componentCollider->id = OBJECT::_08_testWall;
            }

        }

        { // colliders initialization
            {
                u64 meshIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, world.meshesCount, world.meshes, OBJECT::_07_player);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::PLAYER], world.colliders[COLLIDER::ColliderGroup::PLAYER], OBJECT::_07_player);
                COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex], world.meshes[meshIndex], world.transformsCount, world.lTransforms);

            }

            {
                u64 meshIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, world.meshesCount, world.meshes, OBJECT::_08_testWall);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::MAP], world.colliders[COLLIDER::ColliderGroup::MAP], OBJECT::_08_testWall);
                COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::MAP][colliderIndex], world.meshes[meshIndex], world.transformsCount, world.lTransforms);

            }
        }

        { // players
            auto& player = players[0];
            auto& local = player.local;
            player.id = OBJECT::_07_player;
            //
            local.name = "TEST PLAYER1";
            std::vector<InputDevice> controlScheme;
            u64 deviceIndex = 0;
            INPUT_MANAGER::FindDevice(inputManager, InputSource::KEYBOARD, 0, deviceIndex);
            controlScheme.push_back(inputManager->_devices[deviceIndex]);
            inputManager->_devices[deviceIndex].PlayerIndex = 0;
            local.controlScheme = controlScheme;
            u64 transformIndex = 0;
            OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount, world.lTransforms, player.id);
            local.transform = &(world.lTransforms[transformIndex]);
            u64 colliderIndex = 0;
            OBJECT::GetComponentFast<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::PLAYER], world.colliders[COLLIDER::ColliderGroup::PLAYER], player.id);
            local.collider = &(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex]);
        }

        mapGenerator = new MAP_GENERATOR::MapGenerator;
        MAP_GENERATOR::LoadModules(mapGenerator, "test");
        MAP_GENERATOR::GenerateLevel(mapGenerator);

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

		//spdlog::info ("call!");

		//exit (1);

		// Connect Scene to Screen & World structures.
		scene.skybox = &skybox;
		scene.screen = &screen;
		scene.canvas = &canvas;
		scene.world = &world;
	}


	void Destroy () {
        ZoneScopedN("GLOBAL: Destroy");

		for (u64 i = 0; i < screen.parenthoodsCount; ++i) {
			auto& parenthood = screen.parenthoods[i].base;
			delete[] parenthood.children;
		}

		delete[] screen.parenthoods;

		for (u64 i = 0; i < canvas.parenthoodsCount; ++i) {
			auto& parenthood = canvas.parenthoods[i].base;
			delete[] parenthood.children;
		}

		delete[] canvas.parenthoods;

		for (u64 i = 0; i < world.parenthoodsCount; ++i) {
			auto& parenthood = world.parenthoods[i].base;
			delete[] parenthood.children;
		}

		delete[] world.parenthoods;

		DEBUG { spdlog::info ("Destroying mesh components."); }

		RESOURCES::MESHES::DeleteMeshes (
			screen.meshesCount, screen.meshes,
			canvas.meshesCount, canvas.meshes,
			world.meshesCount, world.meshes
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
			world.tables.uniforms, world.tables.meshes, world.materials
		);

		RESOURCES::MATERIALS::DestroyLoadShaders (
			screen.loadTables.shaders, canvas.loadTables.shaders, world.loadTables.shaders
		);

		DEBUG { spdlog::info ("Destroying shader programs."); }

		for (u64 i = 0; i < screen.materialsCount; ++i) {
			auto& material = screen.materials[i];
			SHADER::Destroy (material.program);
		}

		for (u64 i = 0; i < world.materialsCount; ++i) {
			auto& material = world.materials[i];
			SHADER::Destroy (material.program);
		}

        DEBUG { spdlog::info ("Destroying models."); }

        delete[] world.models;

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