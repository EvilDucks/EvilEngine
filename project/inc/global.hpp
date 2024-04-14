#pragma once
#include "types.hpp"

#if PLATFORM == PLATFORM_WINDOWS
#include "platform/win/types.hpp"
#else
#include "platform/agn/types.hpp"
#endif

#include "resources/manager.hpp"
#include "resources/materials.hpp"
#include "resources/meshes.hpp"

#include "scene.hpp"
#include "object.hpp"

#include "render/systems.hpp"
#include "render/texture.hpp"
#include "render/font.hpp"

#include "util/animation.hpp"


//#include "hid/inputManager.hpp"
#include "player/player.hpp"

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
	SCENE::Scene scene   { 0 };
	SCENE::Screen screen { 0 };
	SCENE::Canvas canvas { 0 };
	SCENE::World world   { 0 };

	namespace SU = SHADER::UNIFORM;
	SHADER::UNIFORM::Uniform mat2Uniforms[] { SU::model, SU::view, SU::projection };
	SHADER::UNIFORM::Uniform mat3Uniforms[] { SU::model, SU::view, SU::projection };
	SHADER::UNIFORM::Uniform mat4Uniforms[] { SU::model, SU::view, SU::projection, SU::sampler1 };
	SHADER::UNIFORM::Uniform mat5Uniforms[] { SU::sampler1 };
	SHADER::UNIFORM::Uniform mat6Uniforms[] { SU::sampler1, SU::shift };
	SHADER::UNIFORM::Uniform mat7Uniforms[] { SU::samplerA1, SU::tile };
	SHADER::UNIFORM::Uniform mat8Uniforms[] { SU::projection, SU::color };
	SHADER::UNIFORM::Uniform mat1Uniforms[] { SU::color };

	u8* uniformsTable = nullptr;


	void LoadShaders (
		MATERIAL::Material*& sMaterials, 
		MATERIAL::Material*& cMaterials, 
		MATERIAL::Material*& wMaterials
	) {

		// I need to create a copy inside an dynamic array instead of global varaibles
		// New Array
		// count_byte, shader( count_byte, id_uniformu, ) 

		{
			const u8 uniformSize = sizeof (SHADER::UNIFORM::Uniform);
			DEBUG spdlog::info ("Bytes: {0}", uniformSize);

			const u64 allUniformsTableSize = 1 + 1 + 3 * uniformSize;
			uniformsTable = (u8*) calloc (allUniformsTableSize, sizeof (u8));

			auto&& shadersCount = (uniformsTable + 1);
			auto&& uniformsCount1 = (uniformsTable + 1);
			auto&& uniform1_1 = (SHADER::UNIFORM::Uniform*)(uniformsTable + 2 + uniformSize * 0);
			auto&& uniform2_1 = (SHADER::UNIFORM::Uniform*)(uniformsTable + 2 + uniformSize * 1);
			auto&& uniform3_1 = (SHADER::UNIFORM::Uniform*)(uniformsTable + 2 + uniformSize * 2);

			// WRITE
			*shadersCount = 1;
			*uniformsCount1 = 1;
			*uniform1_1 = SHADER::UNIFORM::model;
			*uniform2_1 = SHADER::UNIFORM::view;
			*uniform3_1 = SHADER::UNIFORM::view;
			// CHECK
			DEBUG spdlog::info ("uniform: {0}, {1}, {2}", (*uniform1_1).id, (*uniform1_1).bufforIndex, (*uniform1_1).setIndex);
			// READ
			DEBUG spdlog::info ("uniform: {0}, {1}, {2}", *(s16*)(uniformsTable + 2), *(u8*)(uniformsTable + 4), *(u8*)(uniformsTable + 5));

			// 

			delete[] uniformsTable;
		}

		using namespace SHADER::UNIFORM;

		const u64 mat2USize = 3;
		const char* mat2UNames[] { NAMES::MODEL, NAMES::VIEW, NAMES::PROJECTION };
		const u64 mat3USize = 3;
		const char* mat3UNames[] { NAMES::MODEL, NAMES::VIEW, NAMES::PROJECTION };
		const u64 mat4USize = 4;
		const char* mat4UNames[] { NAMES::MODEL, NAMES::VIEW, NAMES::PROJECTION, NAMES::SAMPLER_1 };
		const u64 mat5USize = 1;
		const char* mat5UNames[] { NAMES::SAMPLER_1 };
		const u64 mat6USize = 2;
		const char* mat6UNames[] { NAMES::SAMPLER_1, NAMES::SHIFT };
		const u64 mat7USize = 2;
		const char* mat7UNames[] { NAMES::SAMPLER_1, NAMES::TILE };
		const u64 mat8USize = 2;
		const char* mat8UNames[] { NAMES::PROJECTION, NAMES::COLOR };
		const u64 mat1USize = 1;
		const char* mat1UNames[] { NAMES::COLOR };

		{ // SCREEN
			{ // 0
				auto& shader = sMaterials[0].program;
				SHADER::Create (shader, RESOURCES::MANAGER::SVF_S_TEXTURE, RESOURCES::MANAGER::SFF_M_TEXTURE);
				SHADER::UNIFORM::Create (shader, mat6USize, mat6UNames, mat6Uniforms );
			}
			{ // 1
				auto& shader = sMaterials[1].program;
				SHADER::Create (shader, RESOURCES::MANAGER::SVF_S_TEXTURE, RESOURCES::MANAGER::SFF_S_TEXTURE);
				SHADER::UNIFORM::Create (shader, mat5USize, mat5UNames, mat5Uniforms );
			}
			{ // 2
				auto& shader = sMaterials[2].program;
				SHADER::Create (shader, RESOURCES::MANAGER::SVF_ARRAY_TEXTURE, RESOURCES::MANAGER::SFF_ARRAY_TEXTURE);
				SHADER::UNIFORM::Create (shader, mat7USize, mat7UNames, mat7Uniforms );
			} // 3
			{
				auto& shader = sMaterials[3].program;
				SHADER::Create (shader, RESOURCES::MANAGER::svfColorize, RESOURCES::MANAGER::sffColorize);
				SHADER::UNIFORM::Create (shader, mat1USize, mat1UNames, mat1Uniforms );
			}
		}

		{ // CANVAS
			{
				// cMaterials
				auto& shader = FONT::faceShader;
				SHADER::Create (shader, RESOURCES::MANAGER::SVF_FONT, RESOURCES::MANAGER::SFF_FONT);
				SHADER::UNIFORM::Create (shader, mat8USize, mat8UNames, mat8Uniforms );
			}
		}

		{ // WORLD
			{ // 0
				auto& shader = wMaterials[0].program;
				SHADER::Create (shader, RESOURCES::MANAGER::svfWorldA, RESOURCES::MANAGER::sffWorldA);
				SHADER::UNIFORM::Create (shader, mat2USize, mat2UNames, mat2Uniforms );
			}
			{ // 1
				auto& shader = wMaterials[1].program;
				SHADER::Create (shader, RESOURCES::MANAGER::svfWorldTexture, RESOURCES::MANAGER::sffWorldTexture);
				SHADER::UNIFORM::Create (shader, mat4USize, mat4UNames, mat4Uniforms );
			}
		}
	}


	void Initialize () {

		// It's all Data Layer, Memory allocations, Pointer assignments.

		RESOURCES::Json materialsJson;
		RESOURCES::Json meshesJson;
		
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
			screen.materialMeshTable, screen.materialsCount, screen.materials,
			canvas.materialMeshTable, canvas.materialsCount, canvas.materials,
			world.materialMeshTable, world.materialsCount, world.materials
		);

		RESOURCES::MESHES::CreateMeshes (
			meshesJson,
			screen.meshesCount, screen.meshes,
			canvas.meshesCount, canvas.meshes,
			world.meshesCount, world.meshes
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

		DEBUG { spdlog::info ("Creating shader programs."); }

		LoadShaders (screen.materials, canvas.materials, world.materials);

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
			TEXTURE::ARRAY::Create (texture2, textureHolder, GL_RGBA, alphaPixelNoMipmap, writtingAtlas);
			
			textureW0 = texture0;
		}

		DEBUG { spdlog::info ("Creating materials."); }

		RESOURCES::MATERIALS::LoadMaterials (
			materialsJson,
			screen.materialMeshTable, screen.materialsCount, screen.materials,
			canvas.materialMeshTable, canvas.materialsCount, canvas.materials,
			world.materialMeshTable, world.materialsCount, world.materials
		);

		//DEBUG spdlog::info ("a: {0}, b: {1}, c: {2}, d: {3}", screen.materialMeshTable[2], screen.materialMeshTable[4], screen.materialMeshTable[6], screen.materialMeshTable[8]);

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
		scene.world = &world;
	}


	void Destroy () {

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

        DEBUG { spdlog::info ("Destroying camera components."); }

        //delete[] world.camera;

		DEBUG { spdlog::info ("Destroying materials."); }

		RESOURCES::MATERIALS::DestoryMaterials (
			screen.materialMeshTable, screen.materials,
			canvas.materialMeshTable, canvas.materials,
			world.materialMeshTable, world.materials
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
        for (int i = 0; i < playerCount; i++)
        {
            PLAYER::HandlePlayerCollisions(players[i], colliders, collidersCount);
        }
    }

}