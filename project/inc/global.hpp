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


#include "hid/inputManager.hpp"

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
	// To connect to a shader we need a ready to assign array.

	const char unColor[] 		{ "color" };
	const char unModel[] 		{ "model" };
	const char unView[] 		{ "view" };
	const char unProjection[] 	{ "projection" };
	const char unSampler1[]		{ "sampler1" };
	const char unShift[]		{ "shift" };
	const char unTile[]			{ "tile" };

	SHADER::UNIFORM::M4 ubProjection	= glm::mat4(1.0f);
	SHADER::UNIFORM::M4 ubView			= glm::mat4(1.0f);
	SHADER::UNIFORM::M4 ubGlobalSpace	= glm::mat4(1.0f);
	SHADER::UNIFORM::TX ubSampler1 		{ 0, 0 };
	SHADER::UNIFORM::F4 ubColor 		{ 0 };
	SHADER::UNIFORM::F2 ubShift 		{ 0 };
	SHADER::UNIFORM::I1 ubTile 			{ 0 };

	SHADER::UNIFORM::Uniform projection { 0, &ubProjection, 	SHADER::UNIFORM::SetM4 };
	SHADER::UNIFORM::Uniform view 		{ 0, &ubView, 			SHADER::UNIFORM::SetM4 };
	SHADER::UNIFORM::Uniform model 		{ 0, &ubGlobalSpace, 	SHADER::UNIFORM::SetM4 };
	SHADER::UNIFORM::Uniform sampler1 	{ 0, &ubSampler1, 		SHADER::UNIFORM::SetTX };
	SHADER::UNIFORM::Uniform samplerA1	{ 0, &ubSampler1, 		SHADER::UNIFORM::SetAT };
	SHADER::UNIFORM::Uniform color		{ 0, &ubColor, 			SHADER::UNIFORM::SetF4 };
	SHADER::UNIFORM::Uniform shift		{ 0, &ubShift, 			SHADER::UNIFORM::SetF2 };
	SHADER::UNIFORM::Uniform tile		{ 0, &ubTile, 			SHADER::UNIFORM::SetI1 };
	

	const u64 mat1USize = 1;
	SHADER::UNIFORM::Uniform mat1Uniforms[] { color };
	const char* mat1UNames[] { unColor };

	const u64 mat2USize = 3;
	SHADER::UNIFORM::Uniform mat2Uniforms[] { model, view, projection };
	const char* mat2UNames[] { unModel, unView, unProjection };
	
	const u64 mat3USize = 3;
	SHADER::UNIFORM::Uniform mat3Uniforms[] { model, view, projection };
	const char* mat3UNames[] { unModel, unView, unProjection };

	const u64 mat4USize = 4;
	SHADER::UNIFORM::Uniform mat4Uniforms[] { model, view, projection, sampler1 };
	const char* mat4UNames[] { unModel, unView, unProjection, unSampler1 };

	const u64 mat5USize = 1;
	SHADER::UNIFORM::Uniform mat5Uniforms[] { sampler1 };
	const char* mat5UNames[] { unSampler1 };

	const u64 mat6USize = 2;
	SHADER::UNIFORM::Uniform mat6Uniforms[] { sampler1, shift };
	const char* mat6UNames[] { unSampler1, unShift };

	const u64 mat7USize = 2;
	SHADER::UNIFORM::Uniform mat7Uniforms[] { samplerA1, tile };
	const char* mat7UNames[] { unSampler1, unTile };

	const u64 mat8USize = 2;
	SHADER::UNIFORM::Uniform mat8Uniforms[] { projection, color };
	const char* mat8UNames[] { unProjection, unColor };

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
			world.parenthoodsCount = 2; 
			world.transformsCount = 3; // must be 1! (for root)
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
			assert(world.parenthoodsCount == 2);
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
		}

		DEBUG { spdlog::info ("Creating shader programs."); }

		{ // SCREEN
			{ // 0
				auto& shader = screen.materials[0].program;
				SHADER::Create (shader, RESOURCES::MANAGER::SVF_S_TEXTURE, RESOURCES::MANAGER::SFF_M_TEXTURE);
				SHADER::UNIFORM::Create (shader, mat6USize, mat6UNames, mat6Uniforms );
				//SHADER::UNIFORM::Create (shader, mat5USize, mat5UNames, mat5Uniforms );
			}
			{ // 1
				auto& shader = screen.materials[1].program;
				SHADER::Create (shader, RESOURCES::MANAGER::SVF_S_TEXTURE, RESOURCES::MANAGER::SFF_S_TEXTURE);
				SHADER::UNIFORM::Create (shader, mat5USize, mat5UNames, mat5Uniforms );
			}
			{ // 2
				auto& shader = screen.materials[2].program;
				SHADER::Create (shader, RESOURCES::MANAGER::SVF_ARRAY_TEXTURE, RESOURCES::MANAGER::SFF_ARRAY_TEXTURE);
				SHADER::UNIFORM::Create (shader, mat7USize, mat7UNames, mat7Uniforms );
			} // 3
			{
				auto& shader = screen.materials[3].program;
				SHADER::Create (shader, RESOURCES::MANAGER::svfColorize, RESOURCES::MANAGER::sffColorize);
				SHADER::UNIFORM::Create (shader, mat1USize, mat1UNames, mat1Uniforms );
			}
		}

		{ // CANVAS
			{
				auto& shader = FONT::faceShader;
				SHADER::Create (shader, RESOURCES::MANAGER::SVF_FONT, RESOURCES::MANAGER::SFF_FONT);
				SHADER::UNIFORM::Create (shader, mat8USize, mat8UNames, mat8Uniforms );
			}
		}

		{ // WORLD
			{ // 0
				auto& shader = world.materials[0].program;
				SHADER::Create (shader, RESOURCES::MANAGER::svfWorldA, RESOURCES::MANAGER::sffWorldA);
				SHADER::UNIFORM::Create (shader, mat2USize, mat2UNames, mat2Uniforms );
			}
			{ // 1
				auto& shader = world.materials[1].program;
				SHADER::Create (shader, RESOURCES::MANAGER::svfWorldTexture, RESOURCES::MANAGER::sffWorldTexture);
				SHADER::UNIFORM::Create (shader, mat4USize, mat4UNames, mat4Uniforms );
			}
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