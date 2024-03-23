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

	struct Scene {
		/* OTHER */
		u64 materialsCount = 0;
		MATERIAL::Material* materials = nullptr;
		/* COMPONENTS */
		u64 transfromsCount = 0;
		TRANSFORM::Transform* transfroms = nullptr;
		u64 meshesCount = 0;
		MESH::Mesh* meshes = nullptr;
	};

}

namespace GLOBAL {

	Color4 backgroundColor = Color4 ( 114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f );

	WIN::WindowTransform windowTransform { 0, 0, 1200, 640 }; // pos.x, pos.y, size.x, size.y
	WIN::Window mainWindow = nullptr;


	// Shader Vertex FilePath
	#define D_SHADERS "res/shaders/"

	const char* svfSimple 		= D_SHADERS "Simple.vert";
	const char* svfColorize 	= D_SHADERS "Colorize.vert";

	const char* sffSimpleOrange = D_SHADERS "SimpleOrange.frag";
	const char* sffSimpleRed 	= D_SHADERS "SimpleRed.frag";
	const char* sffColorize 	= D_SHADERS "Colorize.frag";


	// SET DURING INITIALIZATION
	// Should be called simply 'scene'
	SCENE::Scene scene { 0 };

	// Collections
	Range<MESH::Base*>* materialMeshes;

	
	// THIS CAN BE LATER MOVED OUTSIDE GLOBAL SPACE into INITIALIZE METHOD using
	//  'SHADER::UNIFORM::Uniform**' and 'const char**'
	//  with a new and a delete call.
	// {

	// Theres a Uniform declaration for each Uniform in Shader.
	//  to apply changes to uniform change it's buffor values.
	const char unColor[] { "color" };
	SHADER::UNIFORM::F4 ubColor1 { 0 }; // unique
	SHADER::UNIFORM::Uniform color { 0, &ubColor1, SHADER::UNIFORM::SetF4 };

	// To connect to a shader we need a ready to assign array.
	SHADER::UNIFORM::Uniform mat1Uniforms[] { color };
	const char* mat1UNames[] { unColor };
	// }


	void Initialize () {
		
		// Data
		scene.materialsCount = 2;
		scene.meshesCount = 2;
		//

		DEBUG { spdlog::info ("Allocating memory for components."); }

		scene.materials = new MATERIAL::Material[scene.materialsCount];
		scene.meshes = new MESH::Mesh[scene.meshesCount] { 0 };

		// With range we can say what elements from said components array
		//  we want to use. 
		//  !!! This information will be always known at compile time. !!!
		//  It gives us the ability to change material for a mesh or range of meshes.
		//  If a mesh is in more then one material mesh-table then it will render that many times.
		//  /* It could be replaced with an array of ranges to reference multiple starting points */
		materialMeshes = new Range<MESH::Base*>[scene.materialsCount] {
			{ 1, &scene.meshes[0].base },
			{ 1, &scene.meshes[1].base },
		};

		DEBUG { spdlog::info ("Creating render materials."); }

		for (u64 i = 0; i < scene.materialsCount; ++i) {
			scene.materials[i].meshes = materialMeshes[i];
		}

		DEBUG { spdlog::info ("Creating shader programs."); }

		{
			auto& shader = scene.materials[0].program;
			SHADER::Create (shader, svfSimple, sffSimpleRed);
		}

		{
			auto& shader = scene.materials[1].program;

			SHADER::Create (shader, svfColorize, sffColorize);
			SHADER::UNIFORM::Create (shader, 1, mat1UNames, mat1Uniforms );
		}

		DEBUG { spdlog::info ("Creating render meshes."); }

		{ // STATIC Square MESH render.
			auto& verticesSize = MESH::DD::SQUARE::VERTICES_COUNT;
			auto& vertices = MESH::DD::SQUARE::VERTICES;
			auto& indicesSize = MESH::DD::SQUARE::INDICES_COUNT;
			auto& indices = MESH::DD::SQUARE::INDICES;
			//
			auto& mesh = scene.meshes[0].base;
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
			auto& mesh = scene.meshes[1].base;
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

	void Destroy () {

		DEBUG { spdlog::info ("Destroying render meshes."); }

		for (u64 i = 0; i < scene.meshesCount; ++i) {
			auto& mesh = scene.meshes[i].base;
			glDeleteVertexArrays (1, &mesh.vao);
			glDeleteBuffers (mesh.buffersCount, mesh.buffers);
		}

		delete[] scene.meshes;


		DEBUG { spdlog::info ("Destroying shader programs."); }

		for (u64 i = 0; i < scene.materialsCount; ++i) {
			auto& material = scene.materials[i];
			SHADER::Destroy (material.program);
		}

		delete[] materialMeshes;
		delete[] scene.materials;

	}


}