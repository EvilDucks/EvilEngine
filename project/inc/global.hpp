#pragma once
#include "types.hpp"

#if PLATFORM == PLATFORM_WINDOWS
#include "platform/win/types.hpp"
#else
#include "platform/agn/types.hpp"
#endif

#include "object.hpp"
#include "scene.hpp"

#include "hid/inputManager.hpp"
#include "hid/input.hpp"

namespace GLOBAL {

	// for x4
	void PrecalcGlobalTransroms (
		const u64& parenthoodsCount, PARENTHOOD::Parenthood* parenthoods,
		const u64& transformsCount, TRANSFORM::Transform* transforms
	);

	Color4 backgroundColor = Color4 ( 114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f );

	WIN::WindowTransform windowTransform { 0, 0, 1200, 640 }; // pos.x, pos.y, size.x, size.y
	WIN::Window mainWindow = nullptr;
    INPUT_MANAGER::IM inputManager = nullptr;
    HID_INPUT::Input input = nullptr;

	// Shader Vertex FilePath
	#define D_SHADERS "res/shaders/"
	#define D_SHADERS_SCREEN D_SHADERS "canvas/"
	#define D_SHADERS_WORLD D_SHADERS "world/"

	const char* svfSimple 		= D_SHADERS_SCREEN "Simple.vert";
	const char* svfColorize 	= D_SHADERS_SCREEN "Colorize.vert";

	const char* sffSimpleOrange = D_SHADERS_SCREEN "SimpleOrange.frag";
	const char* sffSimpleRed 	= D_SHADERS_SCREEN "SimpleRed.frag";
	const char* sffColorize 	= D_SHADERS_SCREEN "Colorize.frag";

	const char* svfWorld 		= D_SHADERS_WORLD "Simple.vert";
	const char* sffWorld 		= D_SHADERS_WORLD "SimpleBlue.frag";


	// SET DURING INITIALIZATION
	SCENE::Scene scene { 0 };
	SCENE::Screen screen { 0 };
	SCENE::World world { 0 };

	// Collections
	Range<MESH::Mesh*>* screenMaterialMeshes;
	Range<MESH::Mesh*>* worldMaterialMeshes;

	
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

		// ! as of right now theres one extra transfrom thats not used ! ENTITY_5

		const u64 ENTITY_1 = 1;
		const u64 ENTITY_2 = 2;
		const u64 ENTITY_3 = 3;
		const u64 ENTITY_4 = 4;
		const u64 ENTITY_5 = 5;
		const u64 ENTITY_6 = 6;
		
		// It's all Data Layer, Memory allocations, Pointer assignments.

		screen.materialsCount = 2;
		screen.meshesCount = 2;
		screen.transformsCount = 1; // must be 1! (for root)

		world.materialsCount = 1;
		world.meshesCount = 2;
		world.transformsCount = 3; // must be 1! (for root)
		world.parenthoodsCount = 2; 

		DEBUG { spdlog::info ("Allocating memory for components."); }

		if (screen.materialsCount)
			screen.materials = new MATERIAL::Material[screen.materialsCount];
		if (screen.meshesCount)
			screen.meshes = new MESH::Mesh[screen.meshesCount] { 0 };

		if (screen.parenthoodsCount)
			screen.parenthoods = new PARENTHOOD::Parenthood[screen.parenthoodsCount] { 0 };
		if (screen.transformsCount)
			screen.transforms = new TRANSFORM::Transform[screen.transformsCount] { 0 };
		
		
		if (world.materialsCount)
			world.materials = new MATERIAL::Material[world.materialsCount];
		if (world.meshesCount)
			world.meshes = new MESH::Mesh[world.meshesCount] { 0 };

		if (world.parenthoodsCount)
			world.parenthoods = new PARENTHOOD::Parenthood[world.parenthoodsCount] { 0 };
		if (world.transformsCount)
			world.transforms = new TRANSFORM::Transform[world.transformsCount] { 0 };
		
		// (NEW) Create parenthood relation 
		//{ // 1 example
		//  assert(world.parenthoodsCount == 1);
		//	auto& compomnentParenthood = world.parenthoods[0];
		//	auto& parenthood = compomnentParenthood.base;
		//	parenthood.childrenCount = 2;
		//	parenthood.children = new GameObjectID[parenthood.childrenCount] {
		//		ENTITY_4, ENTITY_5
		//	};
		//	compomnentParenthood.id = ENTITY_3;
		//}

		// (NEW) Create parenthood relation 
		{ // 2 example
			assert(world.parenthoodsCount == 2);
			{ 
				auto& compomnentParenthood = world.parenthoods[0];
				auto& parenthood = compomnentParenthood.base;
				compomnentParenthood.id = ENTITY_3;
				parenthood.childrenCount = 1;
				parenthood.children = new GameObjectID[parenthood.childrenCount] {
					ENTITY_4
				};
			}
			{
				auto& compomnentParenthood = world.parenthoods[1];
				auto& parenthood = compomnentParenthood.base;
				compomnentParenthood.id = ENTITY_4;
				parenthood.childrenCount = 1;
				parenthood.children = new GameObjectID[parenthood.childrenCount] {
					ENTITY_5
				};
			}
		}

		{ // (NEW) Create Links Material -> Mesh/es 

			/* It could be replaced with an array of ranges to reference multiple starting points */

			screenMaterialMeshes = new Range<MESH::Mesh*>[screen.materialsCount] {
				{ 1, &screen.meshes[0] },
				{ 1, &screen.meshes[1] },
			};

			worldMaterialMeshes = new Range<MESH::Mesh*>[world.materialsCount] {
				{ 2, &world.meshes[0] }
			};
		}

		DEBUG { spdlog::info ("Creating materials."); }

		for (u64 i = 0; i < screen.materialsCount; ++i) {
			screen.materials[i].meshes = screenMaterialMeshes[i];
		}

		for (u64 i = 0; i < world.materialsCount; ++i) {
			world.materials[i].meshes = worldMaterialMeshes[i];
		}

		DEBUG { spdlog::info ("Creating shader programs."); }

		{
			auto& shader = screen.materials[0].program;
			SHADER::Create (shader, svfSimple, sffSimpleRed);
		}

		{
			auto& shader = screen.materials[1].program;
			SHADER::Create (shader, svfColorize, sffColorize);
			SHADER::UNIFORM::Create (shader, mat1USize, mat1UNames, mat1Uniforms );
		}

		{
			auto& shader = world.materials[0].program;
			SHADER::Create (shader, svfWorld, sffWorld);
			SHADER::UNIFORM::Create (shader, mat2USize, mat2UNames, mat2Uniforms );
		}

		DEBUG { spdlog::info ("Creating mesh components."); }

		{ // WORLD

			{ // STATIC Cube MESH render.
				auto& verticesSize = MESH::DDD::CUBE::VERTICES_COUNT;
				auto& vertices = MESH::DDD::CUBE::VERTICES;
				//
				auto& componentMesh = world.meshes[0];
				auto& mesh = componentMesh.base;
				//
				MESH::V::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesSize, vertices
				);
				//
				mesh.verticiesCount = verticesSize;
				mesh.drawFunc = MESH::V::Draw;
				componentMesh.id = ENTITY_3;
			}

			{ // STATIC Square MESH render.
				auto& verticesSize = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& vertices = MESH::DD::SQUARE::VERTICES;
				auto& indicesSize = MESH::DD::SQUARE::INDICES_COUNT;
				auto& indices = MESH::DD::SQUARE::INDICES;
				//
				auto& componentMesh = world.meshes[1];
				auto& mesh = componentMesh.base;
				//
				MESH::VI::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesSize, vertices,
					indicesSize, indices
				);
				//
				mesh.verticiesCount = indicesSize;
				mesh.drawFunc = MESH::VI::Draw;
				componentMesh.id = ENTITY_4;
			}

		}

		{ // Screen

			{ // STATIC Square MESH render.
				auto& verticesSize = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& vertices = MESH::DD::SQUARE::VERTICES;
				auto& indicesSize = MESH::DD::SQUARE::INDICES_COUNT;
				auto& indices = MESH::DD::SQUARE::INDICES;
				//
				auto& componentMesh = screen.meshes[0];
				auto& mesh = componentMesh.base;
				//
				MESH::VI::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesSize, vertices,
					indicesSize, indices
				);
				//
				mesh.verticiesCount = indicesSize;
				mesh.drawFunc = MESH::VI::Draw;
				componentMesh.id = ENTITY_1;
			}

			{ // STATIC Triangle MESH render.
				auto& verticesSize = MESH::DD::TRIANGLE::VERTICES_COUNT;
				auto& vertices = MESH::DD::TRIANGLE::VERTICES;
				//
				auto& componentMesh = screen.meshes[1];
				auto& mesh = componentMesh.base;
				//
				MESH::V::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesSize, vertices
				);
				//
				mesh.verticiesCount = verticesSize;
				mesh.drawFunc = MESH::V::Draw;
				componentMesh.id = ENTITY_2;
			}

		}

		DEBUG { spdlog::info ("Creating transfrom components."); }

		{ // World
			{ // ROOT
				auto& componentTransform = world.transforms[0];
				auto& local = componentTransform.local;
				componentTransform.id = ENTITY_3;
				//
				local.position	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}
			{ 
				auto& componentTransform = world.transforms[1];
				auto& local = componentTransform.local;
				componentTransform.id = ENTITY_4;
				//
				local.position	= glm::vec3 (-1.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 15.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}
			{ 
				auto& componentTransform = world.transforms[2];
				auto& local = componentTransform.local;
				componentTransform.id = ENTITY_5;
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
				componentTransform.id = ENTITY_6;
				//
				local.position	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}

		}

		DEBUG { spdlog::info ("Precalculating transfroms global position."); }

		{ // Precalculate Global Trnasfroms
			PrecalcGlobalTransroms (
				world.parenthoodsCount, world.parenthoods,
				world.transformsCount, world.transforms
			);
			//
			PrecalcGlobalTransroms (
				screen.parenthoodsCount, screen.parenthoods,
				screen.transformsCount, screen.transforms
			);
		}

		// Connect Scene to Screen & World structures.
		scene.screen = &screen;
		scene.world = &world;

		//DEBUG { // Test lol
		//	u64 elementIndex = OBJECT::ID_DEFAULT;
		//	//OBJECT::GetComponentSlow<TRANSFORM::Transform> (
		//	//	elementIndex, world.transformsCount, world.transforms, ENTITY_4
		//	//);
		//	OBJECT::GetComponentFast<TRANSFORM::Transform> (
		//		elementIndex, world.transformsCount, world.transforms, ENTITY_4
		//	);
		//	spdlog::info ("Component Transform Index: {0}", elementIndex);
		//}
	}


	void Destroy () {

		for (u64 i = 0; i < screen.parenthoodsCount; ++i) {
			auto& parenthood = screen.parenthoods[i].base;
			delete[] parenthood.children;
		}

		for (u64 i = 0; i < world.parenthoodsCount; ++i) {
			auto& parenthood = world.parenthoods[i].base;
			delete[] parenthood.children;
		}

		delete[] screen.parenthoods;
		delete[] world.parenthoods;

		DEBUG { spdlog::info ("Destroying mesh components."); }

		for (u64 i = 0; i < screen.meshesCount; ++i) {
			auto& mesh = screen.meshes[i].base;
			glDeleteVertexArrays (1, &mesh.vao);
			glDeleteBuffers (mesh.buffersCount, mesh.buffers);
		}

		delete[] screen.meshes;

		for (u64 i = 0; i < world.meshesCount; ++i) {
			auto& mesh = world.meshes[i].base;
			glDeleteVertexArrays (1, &mesh.vao);
			glDeleteBuffers (mesh.buffersCount, mesh.buffers);
		}

		delete[] world.meshes;

		DEBUG { spdlog::info ("Destroying transfrom components."); }

		delete[] screen.transforms;
		delete[] world.transforms;

		DEBUG { spdlog::info ("Destroying shader programs."); }

		for (u64 i = 0; i < screen.materialsCount; ++i) {
			auto& material = screen.materials[i];
			SHADER::Destroy (material.program);
		}

		for (u64 i = 0; i < world.materialsCount; ++i) {
			auto& material = world.materials[i];
			SHADER::Destroy (material.program);
		}

		DEBUG { spdlog::info ("Destroying materials."); }

		delete[] screenMaterialMeshes;
		delete[] screen.materials;

		delete[] worldMaterialMeshes;
		delete[] world.materials;

	}



	void PrecalcGlobalTransroms (
		const u64& parenthoodsCount,
		PARENTHOOD::Parenthood* parenthoods,
		const u64& transformsCount,
		TRANSFORM::Transform* transforms
	) {
		glm::mat4 localSpace;
		//
		// Root is always 1.0f; One root per canvas/world/screen!
		transforms[0].global = glm::mat4(1.0f);
		//
		for (u64 i = 0; i < parenthoodsCount; ++i) {
			auto& componentParenthood = parenthoods[i];
			auto& parenthood = componentParenthood.base;
			auto& parentId = componentParenthood.id;
			//
			u64 transformIndex = OBJECT::ID_DEFAULT;
			//
			OBJECT::GetComponentFast<TRANSFORM::Transform> (
				transformIndex, transformsCount, transforms, parentId
			);
			//
			auto& pGlobal = transforms[transformIndex].global;
			//
			for (u64 j = 0; j < parenthood.childrenCount; ++j) {
				auto& childId = parenthood.children[j];
				//
				OBJECT::GetComponentFast<TRANSFORM::Transform> (
					transformIndex, transformsCount, transforms, childId
				);
				//
				auto& cComponentTransform = transforms[transformIndex];
				auto& cGlobal = cComponentTransform.global;
				auto& cLocal = cComponentTransform.local;
				//
				localSpace = pGlobal; // Each time copy from parent it's globalspace.
				//
				TRANSFORM::ApplyModel (localSpace, cLocal);
				cGlobal = localSpace;
			}
		}
	}


}