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

#include "hid/inputManager.hpp"

namespace GLOBAL {

	Color4 backgroundColor = Color4 ( 114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f );

	WIN::WindowTransform windowTransform { 0, 0, 1200, 640 }; // pos.x, pos.y, size.x, size.y
    //Prepare starting mouse positions
    float lastX = windowTransform[2] / 2.0f;
    float lastY = windowTransform[3] / 2.0f;
	
    INPUT_MANAGER::IM inputManager = nullptr;
    HID_INPUT::Input input = nullptr;
	WIN::Window mainWindow = nullptr;

	// SET DURING INITIALIZATION
	SCENE::Scene scene   { 0 };
	SCENE::Screen screen { 0 };
	SCENE::Canvas canvas { 0 };
	SCENE::World world   { 0 };
	
	// THIS CAN BE LATER MOVED OUTSIDE GLOBAL SPACE into INITIALIZE METHOD leaving only
	//  'SHADER::UNIFORM::Uniform**' and 'const char**'
	//  with a new and a delete call in Initialize & Delete procedure.
	// {

	// Theres a Uniform declaration for each Uniform in Shader.
	//  to apply changes to uniform change it's buffor values.
	const char unColor[] { "color" };
	SHADER::UNIFORM::F4 ubColor { 0 }; // unique buffer
	SHADER::UNIFORM::Uniform color { 0, &ubColor, SHADER::UNIFORM::SetF4 };

	// To connect to a shader we need a ready to assign array.
	const u64 mat1USize = 1;
	SHADER::UNIFORM::Uniform mat1Uniforms[] { color };
	const char* mat1UNames[] { unColor };


	const char unModel[] 		{ "model" };
	const char unView[] 		{ "view" };
	const char unProjection[] 	{ "projection" };

	SHADER::UNIFORM::M4 ubProjection = glm::mat4(1.0f); // unique buffer 
	SHADER::UNIFORM::M4 ubView = glm::mat4(1.0f); // unique buffer
	SHADER::UNIFORM::M4 ubGlobalSpace = glm::mat4(1.0f); // unique buffer "Should not be unique?"

	SHADER::UNIFORM::Uniform projection { 0, &ubProjection, SHADER::UNIFORM::SetM4 };
	SHADER::UNIFORM::Uniform view { 0, &ubView, SHADER::UNIFORM::SetM4 };
	SHADER::UNIFORM::Uniform model { 0, &ubGlobalSpace, SHADER::UNIFORM::SetM4 };

	const u64 mat2USize = 3;
	SHADER::UNIFORM::Uniform mat2Uniforms[] { model, view, projection };
	const char* mat2UNames[] { unModel, unView, unProjection };

	// }


	void Initialize () {

		// It's all Data Layer, Memory allocations, Pointer assignments.

		RESOURCES::Json materialsJson;
		RESOURCES::Json meshesJson;
		
		{ // SCREEN
			screen.parenthoodsCount = 0; 
			screen.transformsCount = 1; // must be 1! (for root)
		}

		{ // SCREEN
			canvas.parenthoodsCount = 0; 
			canvas.transformsCount = 0;
		}
		
		{ // WORLD
			world.parenthoodsCount = 3;
			world.transformsCount = 4; // must be 1! (for root)
		}
		

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
			
			// 2 example
			assert(world.parenthoodsCount == 3);
			{  
				auto& compomnentParenthood = world.parenthoods[0];
				auto& parenthood = compomnentParenthood.base;
				compomnentParenthood.id = OBJECT::_3;
				parenthood.childrenCount = 1;
				parenthood.children = new GameObjectID[parenthood.childrenCount] {
					OBJECT::_4
				};
			}
			{
				auto& compomnentParenthood = world.parenthoods[1];
				auto& parenthood = compomnentParenthood.base;
				compomnentParenthood.id = OBJECT::_4;
				parenthood.childrenCount = 1;
				parenthood.children = new GameObjectID[parenthood.childrenCount] {
					OBJECT::_5
				};
			}
            {
                auto& compomnentParenthood = world.parenthoods[2];
                auto& parenthood = compomnentParenthood.base;
                compomnentParenthood.id = OBJECT::_player;
                parenthood.childrenCount = 0;

            }
		}

		DEBUG { spdlog::info ("Creating shader programs."); }

		{ // SCREEN
			{ // 0
				auto& shader = screen.materials[0].program;
				SHADER::Create (shader, RESOURCES::MANAGER::svfSimple, RESOURCES::MANAGER::sffSimpleRed);
			}
			{ // 1
				auto& shader = screen.materials[1].program;
				SHADER::Create (shader, RESOURCES::MANAGER::svfColorize, RESOURCES::MANAGER::sffColorize);
				SHADER::UNIFORM::Create (shader, mat1USize, mat1UNames, mat1Uniforms );
			}
		}

		{ // CANVAS

		}

		{ // WORLD
			{ // 0
				auto& shader = world.materials[0].program;
				SHADER::Create (shader, RESOURCES::MANAGER::svfWorld, RESOURCES::MANAGER::sffWorld);
				SHADER::UNIFORM::Create (shader, mat2USize, mat2UNames, mat2Uniforms );
			}
		}

		DEBUG { spdlog::info ("Creating materials."); }

		RESOURCES::MATERIALS::LoadMaterials (
			materialsJson,
			screen.materialMeshTable, screen.materialsCount, screen.materials,
			canvas.materialMeshTable, canvas.materialsCount, canvas.materials,
			world.materialMeshTable, world.materialsCount, world.materials
		);

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
				componentTransform.id = OBJECT::_5;
				//
				local.position	= glm::vec3 (2.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}
            {
                auto& componentTransform = world.transforms[3];
                auto& local = componentTransform.local;
                componentTransform.id = OBJECT::_player;
                //
                local.position	= glm::vec3 (0.0f, 0.0f, -1.0f);
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

                updateCameraVectors(world.camera);
            }
        }
		DEBUG { spdlog::info ("Precalculating transfroms global position."); }

		{ // Precalculate Global Trnasfroms
			RENDER::SYSTEMS::PrecalculateGlobalTransroms (
				world.parenthoodsCount, world.parenthoods,
				world.transformsCount, world.transforms
			);
			//
			RENDER::SYSTEMS::PrecalculateGlobalTransroms (
				screen.parenthoodsCount, screen.parenthoods,
				screen.transformsCount, screen.transforms
			);
		}

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

}