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

    PLAYER::Player *players = nullptr;
    u64 playerCount = 0;

	// SET DURING INITIALIZATION
	SCENE::Scene scene { 0 };
	SCENE::Screen screen { 0 };
	SCENE::Canvas canvas { 0 };
	SCENE::World world   { 0 };

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
			world.parenthoodsCount = 2;
			world.transformsCount = 5; // must be 1! (for root)
            world.collidersCount[COLLIDER::ColliderGroup::PLAYER] = 1;
            world.collidersCount[COLLIDER::ColliderGroup::MAP] = 1;
		}
		playerCount = 1;

		DEBUG { spdlog::info ("Allocating memory for components and collections."); }

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

		RESOURCES::SCENE::Create (
			sceneJson
		);

		{ // SCREEN
			if (screen.parenthoodsCount) screen.parenthoods = new PARENTHOOD::Parenthood[screen.parenthoodsCount] { 0 };
			if (screen.transformsCount) screen.transforms = new TRANSFORM::Transform[screen.transformsCount] { 0 };
		}

		{ // CANVAS
			if (canvas.parenthoodsCount) canvas.parenthoods = new PARENTHOOD::Parenthood[canvas.parenthoodsCount] { 0 };
			if (canvas.transformsCount) canvas.transforms = new TRANSFORM::Transform[canvas.transformsCount] { 0 };
		}

		{ // WORLD
			if (world.parenthoodsCount) world.parenthoods = new PARENTHOOD::Parenthood[world.parenthoodsCount] { 0 };
			if (world.transformsCount) world.transforms = new TRANSFORM::Transform[world.transformsCount] { 0 };
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
                componentParenthood.id = OBJECT::_3;
				parenthood.childrenCount = 3;
				parenthood.children = new GameObjectID[parenthood.childrenCount] {
					OBJECT::_4, OBJECT::_testWall, OBJECT::_player
				};
			}
			{
				auto& componentParenthood = world.parenthoods[1];
				auto& parenthood = componentParenthood.base;
                componentParenthood.id = OBJECT::_4;
				parenthood.childrenCount = 1;
				parenthood.children = new GameObjectID[parenthood.childrenCount] {
					OBJECT::_5
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
			const TEXTURE::Properties textureRGBA { GL_RGBA8, 0, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR };
			const TEXTURE::Properties textureRGB { GL_RGB8, 0, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST };
			const TEXTURE::Properties alphaPixelNoMipmap { GL_RGBA8, 1, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST };

			const TEXTURE::Atlas dustsAtlas { 6, 6, 1, 16, 16 }; // elements, cols, rows, tile_pixels_x, tile_pixels_y
			const TEXTURE::Atlas writtingAtlas { 6, 5, 2, 64, 64 };

			auto& texture0 = screen.materials[0].texture;
			auto& texture1 = screen.materials[1].texture;
			auto& texture2 = screen.materials[2].texture;
			auto& textureW0 = world.materials[1].texture;
			
			TEXTURE::Holder textureHolder;

			stbi_set_flip_vertically_on_load (true);

			TEXTURE::Load (textureHolder, RESOURCES::MANAGER::TEXTURE_BRICK);
			TEXTURE::SINGLE::Create (texture0, textureHolder, GL_RGB, textureRGB);

			TEXTURE::Load (textureHolder, RESOURCES::MANAGER::TEXTURE_TIN_SHEARS);
			TEXTURE::SINGLE::Create (texture1, textureHolder, GL_RGB, textureRGB);

			//TEXTURE::Load (textureHolder, RESOURCES::MANAGER::ANIMATED_TEXTURE_1);
			//TEXTURE::ARRAY::Create (texture0, textureHolder, GL_RGBA, alphaPixelNoMipmap, dustsAtlas);
			TEXTURE::Load (textureHolder, RESOURCES::MANAGER::ANIMATED_TEXTURE_2);
			TEXTURE::ARRAY::Create2 (texture2, textureHolder, GL_RGBA, alphaPixelNoMipmap, writtingAtlas);
			
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

		RESOURCES::SHADERS::LoadShaders ( 19, D_SHADERS_SCREEN, screen.loadTables.shaders, screen.tables.uniforms, screen.materials );
		//DEBUG_RENDER GL::GetError (1234);
		//RESOURCES::SHADERS::LoadShaders ( 19, D_SHADERS_CANVAS, canvas.loadTables.shaders, canvas.tables.uniforms, canvas.materials );
		RESOURCES::SHADERS::LoadShadersCanvas (canvas.tables.uniforms, canvas.materials);
		//DEBUG_RENDER GL::GetError (1235);
		RESOURCES::SHADERS::LoadShaders ( 18, D_SHADERS_WORLD, world.loadTables.shaders, world.tables.uniforms, world.materials );
		//DEBUG_RENDER GL::GetError (1236);

		DEBUG { spdlog::info ("Creating meshes."); }

		RESOURCES::MESHES::LoadMeshes (
			meshesJson,
			screen.meshesCount, screen.meshes,
			canvas.meshesCount, canvas.meshes,
			world.meshesCount, world.meshes
		);

		DEBUG { spdlog::info ("Creating transfrom components."); }

		{ // World
			{ // ROOT
				auto& componentTransform = world.transforms[0];
				auto& local = componentTransform.local;
				componentTransform.id = OBJECT::_3;
				//
				local.position	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}
			{ 
				auto& componentTransform = world.transforms[1];
				auto& local = componentTransform.local;
				componentTransform.id = OBJECT::_4;
				//
				local.position	= glm::vec3 (-1.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 15.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}
			{
                auto& componentTransform = world.transforms[2];
                auto& local = componentTransform.local;
                componentTransform.id = OBJECT::_testWall;
                //
                local.position	= glm::vec3 (0.0f, 0.0f, -10.0f);
                local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
                local.scale		= glm::vec3 (5.0f, 3.0f, 0.5f);
            }
            {
                auto& componentTransform = world.transforms[3];
                auto& local = componentTransform.local;
                componentTransform.id = OBJECT::_player;
                //
                local.position	= glm::vec3 (0.0f, 0.0f, 2.0f);
                local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
                local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
            }
			{ 
				auto& componentTransform = world.transforms[4];
				auto& local = componentTransform.local;
				componentTransform.id = OBJECT::_5;
				//
				local.position	= glm::vec3 (2.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}
		}

		{ // Screen

			{ // ROOT
				auto& componentTransform = screen.transforms[0];
				auto& local = componentTransform.local;
				componentTransform.id = OBJECT::_6;
				//
				local.position	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}

		}

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
		DEBUG { spdlog::info ("Precalculating transfroms global position."); }

		{ // Precalculate Global Trnasfroms
            RENDER::SYSTEMS::PrecalculateGlobalTransforms(
                    world.parenthoodsCount, world.parenthoods,
                    world.transformsCount, world.transforms
            );
			//
            RENDER::SYSTEMS::PrecalculateGlobalTransforms(
                    screen.parenthoodsCount, screen.parenthoods,
                    screen.transformsCount, screen.transforms
            );

			auto& transform = world.transforms[2];
			//spdlog::info (
			//	"Transform:\n"
			//	"{0}, {1}, {2}, {3}\n"
			//	"{4}, {5}, {6}, {7}\n"
			//	"{8}, {9}, {10}, {11}\n"
			//	"{12}, {13}, {14}, {15}", 
			//	transform.global[0][0], transform.global[0][1], transform.global[0][2], transform.global[0][3],
			//	transform.global[1][0], transform.global[1][1], transform.global[1][2], transform.global[1][3],
			//	transform.global[2][0], transform.global[2][1], transform.global[2][2], transform.global[2][3],
			//	transform.global[3][0], transform.global[3][1], transform.global[3][2], transform.global[3][3]
			//);
		}

        // COLLIDERS
        {// world colliders
            {
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::PLAYER];
                auto& local = componentCollider->local;
                local.group = COLLIDER::ColliderGroup::PLAYER;
                local.type = COLLIDER::ColliderType::AABB;
                componentCollider->id = OBJECT::_player;
            }

            {
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::MAP];
                auto& local = componentCollider->local;
                local.group = COLLIDER::ColliderGroup::MAP;
                local.type = COLLIDER::ColliderType::AABB;
                componentCollider->id = OBJECT::_testWall;
            }

        }

        { // colliders initialization
            {
                u64 meshIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, world.meshesCount, world.meshes, OBJECT::_player);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::PLAYER], world.colliders[COLLIDER::ColliderGroup::PLAYER], OBJECT::_player);
                COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex], world.meshes[meshIndex], world.transformsCount, world.transforms);

            }

            {
                u64 meshIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, world.meshesCount, world.meshes, OBJECT::_testWall);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::MAP], world.colliders[COLLIDER::ColliderGroup::MAP], OBJECT::_testWall);
                COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::MAP][colliderIndex], world.meshes[meshIndex], world.transformsCount, world.transforms);

            }
        }

        { // players
            auto& player = players[0];
            auto& local = player.local;
            player.id = OBJECT::_player;
            //
            local.name = "TEST PLAYER1";
            std::vector<InputDevice> controlScheme;
            u64 deviceIndex = 0;
            INPUT_MANAGER::FindDevice(inputManager, InputSource::KEYBOARD, 0, deviceIndex);
            controlScheme.push_back(inputManager->_devices[deviceIndex]);
            inputManager->_devices[deviceIndex].playerIndex = 0;
            local.controlScheme = controlScheme;
            u64 transformIndex = 0;
            OBJECT::GetComponentFast<TRANSFORM::Transform>(transformIndex, world.transformsCount, world.transforms, player.id);
            local.transform = &(world.transforms[transformIndex]);
            u64 colliderIndex = 0;
            OBJECT::GetComponentFast<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::PLAYER], world.colliders[COLLIDER::ColliderGroup::PLAYER], player.id);
            local.collider = &(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex]);
        }
        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		// Connect Scene to Screen & World structures.
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

		delete[] screen.transforms;
		delete[] canvas.transforms;
		delete[] world.transforms;

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