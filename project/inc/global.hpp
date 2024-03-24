#pragma once

#include "types.hpp"

#if PLATFORM == PLATFORM_WINDOWS
#include "platform/win/types.hpp"
#else
#include "platform/agn/types.hpp"
#endif

#include "render/mesh.hpp"
#include "render/material.hpp"
#include "render/transform.hpp"


namespace SCENE {

	struct Canvas {
		/* OTHER */
		u64 materialsCount = 0;
		MATERIAL::Material* materials = nullptr;
		/* COMPONENTS */
		u64 transfromsCount = 0;
		TRANSFORM::Transform* transfroms = nullptr;
		u64 meshesCount = 0;
		MESH::Mesh* meshes = nullptr;
	};

	struct World {
		/* OTHER */
		u64 materialsCount = 0;
		MATERIAL::Material* materials = nullptr;
		/* COMPONENTS */
		u64 transfromsCount = 0;
		TRANSFORM::Transform* transfroms = nullptr;
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
	Range<MESH::Base*>* canvasMaterialMeshes;
	Range<MESH::Base*>* worldMaterialMeshes;

	
	// THIS CAN BE LATER MOVED OUTSIDE GLOBAL SPACE into INITIALIZE METHOD leaving only
	//  'SHADER::UNIFORM::Uniform**' and 'const char**'
	//  with a new and a delete call in Initialize & Delete procedure.
	// {

	// Theres a Uniform declaration for each Uniform in Shader.
	//  to apply changes to uniform change it's buffor values.
	const char unColor[] { "color" };
	SHADER::UNIFORM::F4 ubColor1 { 0 }; // unique buffer
	SHADER::UNIFORM::Uniform color { 0, &ubColor1, SHADER::UNIFORM::SetF4 };

	// To connect to a shader we need a ready to assign array.
	SHADER::UNIFORM::Uniform mat1Uniforms[] { color };
	const char* mat1UNames[] { unColor };


	const char unModel[] 		{ "model" };
	const char unView[] 		{ "view" };
	const char unProjection[] 	{ "projection" };

	SHADER::UNIFORM::M4 ubModel1 = glm::mat4(1.0f); // unique buffer
	SHADER::UNIFORM::M4 ubView1 = glm::mat4(1.0f); // unique buffer
	SHADER::UNIFORM::M4 ubProjection1 = glm::mat4(1.0f); // unique buffer "Should not be unique!"

	SHADER::UNIFORM::Uniform model { 0, &ubModel1, SHADER::UNIFORM::SetM4 };
	SHADER::UNIFORM::Uniform view { 0, &ubView1, SHADER::UNIFORM::SetM4 };
	SHADER::UNIFORM::Uniform projection { 0, &ubProjection1, SHADER::UNIFORM::SetM4 };

	SHADER::UNIFORM::Uniform mat2Uniforms[] { model, view, projection };
	const char* mat2UNames[] { unModel, unView, unProjection };

	// }


	void Initialize () {
		
		// Data
		canvas.materialsCount = 2;
		canvas.meshesCount = 2;

		world.materialsCount = 1;
		world.meshesCount = 1;
		//

		DEBUG { spdlog::info ("Allocating memory for components."); }

		canvas.materials = new MATERIAL::Material[canvas.materialsCount];
		canvas.meshes = new MESH::Mesh[canvas.meshesCount] { 0 };

		world.materials = new MATERIAL::Material[world.materialsCount];
		world.meshes = new MESH::Mesh[world.materialsCount] { 0 };

		{ /* It could be replaced with an array of ranges to reference multiple starting points */

			// Create Links Material -> Mesh/es

			canvasMaterialMeshes = new Range<MESH::Base*>[canvas.materialsCount] {
				{ 1, &canvas.meshes[0].base },
				{ 1, &canvas.meshes[1].base },
			};

			worldMaterialMeshes = new Range<MESH::Base*>[world.materialsCount] {
				{ 1, &world.meshes[0].base }
			};
		}

		DEBUG { spdlog::info ("Creating render materials."); }

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
			SHADER::UNIFORM::Create (shader, 1, mat1UNames, mat1Uniforms );
		}

		{
			auto& shader = world.materials[0].program;
			SHADER::Create (shader, svfWorld, sffWorld);
			SHADER::UNIFORM::Create (shader, 3, mat2UNames, mat2Uniforms );
		}

		DEBUG { spdlog::info ("Creating render meshes."); }

		{ // CANVAS

			{ // STATIC Square MESH render.
				auto& verticesSize = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& vertices = MESH::DD::SQUARE::VERTICES;
				auto& indicesSize = MESH::DD::SQUARE::INDICES_COUNT;
				auto& indices = MESH::DD::SQUARE::INDICES;
				//
				auto& mesh = canvas.meshes[0].base;
				//
				MESH::DD::VI::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesSize, vertices,
					indicesSize, indices
				);
				//
				mesh.verticiesCount = indicesSize;
				mesh.drawFunc = MESH::DD::VI::Draw;
			}

			{ // STATIC Triangle MESH render.
				auto& verticesSize = MESH::DD::TRIANGLE::VERTICES_COUNT;
				auto& vertices = MESH::DD::TRIANGLE::VERTICES;
				//
				auto& mesh = canvas.meshes[1].base;
				//
				MESH::DD::V::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesSize, vertices
				);
				//
				mesh.verticiesCount = verticesSize;
				mesh.drawFunc = MESH::DD::V::Draw;
			}

		}

		{ // WORLD

			//{ // STATIC Square MESH render.
			//	auto& verticesSize = MESH::DD::SQUARE::VERTICES_COUNT;
			//	auto& vertices = MESH::DD::SQUARE::VERTICES;
			//	auto& indicesSize = MESH::DD::SQUARE::INDICES_COUNT;
			//	auto& indices = MESH::DD::SQUARE::INDICES;
			//	//
			//	auto& mesh = world.meshes[0].base;
			//	//
			//	MESH::DD::VI::CreateVAO (
			//		mesh.vao, mesh.buffers,
			//		verticesSize, vertices,
			//		indicesSize, indices
			//	);
			//	//
			//	mesh.verticiesCount = indicesSize;
			//	mesh.drawFunc = MESH::DD::VI::Draw;
			//}

			{ // STATIC Cube MESH render.
				auto& verticesSize = MESH::DDD::CUBE::VERTICES_COUNT;
				auto& vertices = MESH::DDD::CUBE::VERTICES;
				//
				auto& mesh = world.meshes[0].base;
				//
				MESH::DD::V::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesSize, vertices
				);
				//
				mesh.verticiesCount = verticesSize;
				mesh.drawFunc = MESH::DD::V::Draw;
			}

		}

		// Connect Scene to Canvas & World structures.
		scene.canvas = &canvas;
		scene.world = &world;

	}

	void Destroy () {

		DEBUG { spdlog::info ("Destroying render meshes."); }

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

		DEBUG { spdlog::info ("Destroying shader programs."); }

		for (u64 i = 0; i < canvas.materialsCount; ++i) {
			auto& material = canvas.materials[i];
			SHADER::Destroy (material.program);
		}

		delete[] canvasMaterialMeshes;
		delete[] canvas.materials;

		for (u64 i = 0; i < world.materialsCount; ++i) {
			auto& material = world.materials[i];
			SHADER::Destroy (material.program);
		}

		delete[] worldMaterialMeshes;
		delete[] world.materials;

	}


}