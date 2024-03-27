#pragma once

#include "types.hpp"

#if PLATFORM == PLATFORM_WINDOWS
#include "platform/win/types.hpp"
#else
#include "platform/agn/types.hpp"
#endif

#include "object.hpp"
#include "render/mesh.hpp"
#include "render/material.hpp"
#include "render/transform.hpp"
#include "hid/inputManager.hpp"
#include "hid/input.hpp"


namespace SCENE {

	struct Canvas {
		/* OTHER */
		u64 materialsCount = 0;
		MATERIAL::Material* materials = nullptr;
		/* COMPONENTS */
		u64 transformsCount = 0;
		TRANSFORM::Transform* transforms = nullptr;
		u64 meshesCount = 0;
		MESH::Mesh* meshes = nullptr;
	};

	struct World {
		/* OTHER */
		u64 materialsCount = 0;
		MATERIAL::Material* materials = nullptr;
		/* COMPONENTS */
		u64 transformsCount = 0;
		TRANSFORM::Transform* transforms = nullptr;
		u64 meshesCount = 0;
		MESH::Mesh* meshes = nullptr;
	};

	struct Scene {
		Canvas* canvas = nullptr;
		World* world = nullptr;
	};

}

namespace GLOBAL {

	Color4 backgroundColor = Color4 ( 114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f );

	WIN::WindowTransform windowTransform { 0, 0, 1200, 640 }; // pos.x, pos.y, size.x, size.y
	WIN::Window mainWindow = nullptr;
    INPUT_MANAGER::IM inputManager = nullptr;
    HID_INPUT::Input input = nullptr;

	// Shader Vertex FilePath
	#define D_SHADERS "res/shaders/"
	#define D_SHADERS_CANVAS D_SHADERS "canvas/"
	#define D_SHADERS_WORLD D_SHADERS "world/"

	const char* svfSimple 		= D_SHADERS_CANVAS "Simple.vert";
	const char* svfColorize 	= D_SHADERS_CANVAS "Colorize.vert";

	const char* sffSimpleOrange = D_SHADERS_CANVAS "SimpleOrange.frag";
	const char* sffSimpleRed 	= D_SHADERS_CANVAS "SimpleRed.frag";
	const char* sffColorize 	= D_SHADERS_CANVAS "Colorize.frag";

	const char* svfWorld 		= D_SHADERS_WORLD "Simple.vert";
	const char* sffWorld 		= D_SHADERS_WORLD "SimpleBlue.frag";


	// SET DURING INITIALIZATION
	SCENE::Scene scene { 0 };
	SCENE::Canvas canvas { 0 };
	SCENE::World world { 0 };

	// Collections
	Range<MESH::Mesh*>* canvasMaterialMeshes;
	Range<MESH::Mesh*>* worldMaterialMeshes;


	// Without using collection determine based on meshes which transforms to pick.
	u64 transfromsMeshesCount = 2;
	u64 transfromsMeshesLineSize = 0;
	u64 transformsMeshesGapSize = 0;
	u64 transformsMeshesOffset = 0;

	
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

		const u64 ENTITY_1 = 1;
		const u64 ENTITY_2 = 2;
		const u64 ENTITY_3 = 3;
		const u64 ENTITY_4 = 4;
		const u64 ENTITY_5 = 5;
		
		// It's all Data Layer, Memory allocations, Pointer assignments.

		canvas.materialsCount = 2;
		canvas.meshesCount = 2;
		canvas.transformsCount = 0;

		world.materialsCount = 1;
		world.meshesCount = 2;
		world.transformsCount = 3;

		DEBUG { spdlog::info ("Allocating memory for components."); }

		if (canvas.materialsCount)
			canvas.materials = new MATERIAL::Material[canvas.materialsCount];
		if (canvas.transformsCount)
			canvas.transforms = new TRANSFORM::Transform[canvas.transformsCount] { 0 };
		if (canvas.meshesCount)
			canvas.meshes = new MESH::Mesh[canvas.meshesCount] { 0 };
		
		if (world.materialsCount)
			world.materials = new MATERIAL::Material[world.materialsCount];
		if (world.transformsCount)
			world.transforms = new TRANSFORM::Transform[world.transformsCount] { 0 };
		if (world.meshesCount)
			world.meshes = new MESH::Mesh[world.meshesCount] { 0 };
		

		{ /* It could be replaced with an array of ranges to reference multiple starting points */

			// Create Links Material -> Mesh/es

			canvasMaterialMeshes = new Range<MESH::Mesh*>[canvas.materialsCount] {
				{ 1, &canvas.meshes[0] },
				{ 1, &canvas.meshes[1] },
			};

			worldMaterialMeshes = new Range<MESH::Mesh*>[world.materialsCount] {
				{ 2, &world.meshes[0] }
			};
		}

		DEBUG { spdlog::info ("Creating materials."); }

		for (u64 i = 0; i < canvas.materialsCount; ++i) {
			canvas.materials[i].meshes = canvasMaterialMeshes[i];
		}

		for (u64 i = 0; i < world.materialsCount; ++i) {
			world.materials[i].meshes = worldMaterialMeshes[i];
		}

		DEBUG { spdlog::info ("Creating shader programs."); }

		{
			auto& shader = canvas.materials[0].program;
			SHADER::Create (shader, svfSimple, sffSimpleRed);
		}

		{
			auto& shader = canvas.materials[1].program;
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

		{ // CANVAS

			{ // STATIC Square MESH render.
				auto& verticesSize = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& vertices = MESH::DD::SQUARE::VERTICES;
				auto& indicesSize = MESH::DD::SQUARE::INDICES_COUNT;
				auto& indices = MESH::DD::SQUARE::INDICES;
				//
				auto& componentMesh = canvas.meshes[0];
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
				auto& componentMesh = canvas.meshes[1];
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
			{ 
				auto& componentTransform = world.transforms[0];
				auto& local = componentTransform.local;
				//
				local.position	= glm::vec3 (0.0f, 1.0f, 0.0f);
				local.rotation	= glm::vec3 (15.0f, 25.0f, 35.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
				//
				componentTransform.id = ENTITY_3;
			}
			{ 
				auto& componentTransform = world.transforms[1];
				auto& local = componentTransform.local;
				//
				local.position	= glm::vec3 (1.0f, 0.0f, 0.0f);
				local.rotation	= glm::vec3 (15.0f, 25.0f, 35.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
				//
				componentTransform.id = ENTITY_4;
			}
			{ 
				auto& componentTransform = world.transforms[2];
				auto& local = componentTransform.local;
				//
				local.position	= glm::vec3 (-1.0f, 1.0f, 0.0f);
				local.rotation	= glm::vec3 (15.0f, 25.0f, 35.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
				//
				componentTransform.id = ENTITY_5;
			}
		}

		DEBUG { spdlog::info ("Precalculating transfroms global position."); }

		{ // Precalculate Global Trnasfroms
			glm::mat4 globalSpace;
			//
			for (u64 i = 0; i < world.transformsCount; ++i) {
				auto& componentTransform = world.transforms[i];
				auto& global = componentTransform.global;
				auto& local = componentTransform.local;
				//
				globalSpace = glm::mat4(1.0f);
				TRANSFORM::ApplyModel (globalSpace, local);
				//
				componentTransform.global = globalSpace;
			}
			//
			for (u64 i = 0; i < canvas.transformsCount; ++i) {
				auto& componentTransform = canvas.transforms[i];
				auto& global = componentTransform.global;
				auto& local = componentTransform.local;
				//
				globalSpace = glm::mat4(1.0f);
				TRANSFORM::ApplyModel (globalSpace, local);
				//
				componentTransform.global = globalSpace;
			}
		}

		// Connect Scene to Canvas & World structures.
		scene.canvas = &canvas;
		scene.world = &world;

		DEBUG { // Test lol
			u64 elementIndex = OBJECT::ID_DEFAULT;
			//OBJECT::GetComponentSlow<TRANSFORM::Transform> (
			//	elementIndex, world.transformsCount, world.transforms, ENTITY_4
			//);
			OBJECT::GetComponentFast<TRANSFORM::Transform> (
				elementIndex, world.transformsCount, world.transforms, ENTITY_4
			);
			spdlog::info ("Component Transform Index: {0}", elementIndex);
		}
		


	}


	void Destroy () {

		DEBUG { spdlog::info ("Destroying mesh components."); }

		for (u64 i = 0; i < canvas.meshesCount; ++i) {
			auto& mesh = canvas.meshes[i].base;
			glDeleteVertexArrays (1, &mesh.vao);
			glDeleteBuffers (mesh.buffersCount, mesh.buffers);
		}

		delete[] canvas.meshes;

		for (u64 i = 0; i < world.meshesCount; ++i) {
			auto& mesh = world.meshes[i].base;
			glDeleteVertexArrays (1, &mesh.vao);
			glDeleteBuffers (mesh.buffersCount, mesh.buffers);
		}

		delete[] world.meshes;

		DEBUG { spdlog::info ("Destroying transfrom components."); }

		delete[] canvas.transforms;
		delete[] world.transforms;

		DEBUG { spdlog::info ("Destroying shader programs."); }

		for (u64 i = 0; i < canvas.materialsCount; ++i) {
			auto& material = canvas.materials[i];
			SHADER::Destroy (material.program);
		}

		for (u64 i = 0; i < world.materialsCount; ++i) {
			auto& material = world.materials[i];
			SHADER::Destroy (material.program);
		}

		DEBUG { spdlog::info ("Destroying materials."); }

		delete[] canvasMaterialMeshes;
		delete[] canvas.materials;

		delete[] worldMaterialMeshes;
		delete[] world.materials;

	}


}