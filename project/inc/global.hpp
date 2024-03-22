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


namespace SCENES {

	struct SceneTree {
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

	WIN::WindowTransform windowTransform { 0, 0, 1200, 640 };
	WIN::Window mainWindow = nullptr;

	// Shader Vertex FilePath
	#define D_SHADERS "res/shaders/"

	const char* svfSimple 		= D_SHADERS "Simple.vert";
	const char* svfColorize 	= D_SHADERS "Colorize.vert";

	const char* sffSimpleOrange = D_SHADERS "SimpleOrange.frag";
	const char* sffSimpleRed 	= D_SHADERS "SimpleRed.frag";
	const char* sffColorize 	= D_SHADERS "Colorize.frag";

	// SET DURING INITIALIZATION
	SCENES::SceneTree sceneTree { 0 };

	Range<MESH::Base*>* materialMeshes;
	const char uniNameColor[] { "color" };
	const char* uniNames[] { 
		uniNameColor
	};

	SHADER::SetFunc setFuncs[] {
		SHADER::SetColor
	};

	void Initialize () {
		
		// Data
		sceneTree.materialsCount = 2;
        sceneTree.meshesCount = 2;
		//

		DEBUG { spdlog::info ("Allocating memory for components."); }

		sceneTree.materials = new MATERIAL::Material[sceneTree.materialsCount];
        sceneTree.meshes = new MESH::Mesh[sceneTree.meshesCount] { 0 };

		// With range we can say what elements from said components array
		//  we want to use. 
		//  !!! This information will be always known at compile time. !!!
		//  It gives us the ability to change material for a mesh or range of meshes.
		//  If a mesh is in more then one material mesh-table then it will render that many times.
		//  /* It could be replaced with an array of ranges to reference multiple starting points */
		materialMeshes = new Range<MESH::Base*>[sceneTree.materialsCount] {
			{ 1, &sceneTree.meshes[0].base },
			{ 1, &sceneTree.meshes[1].base },
		};

		DEBUG { spdlog::info ("Creating render materials."); }

		for (u64 i = 0; i < sceneTree.materialsCount; ++i) {
			sceneTree.materials[i].meshes = materialMeshes[i];
		}

		DEBUG { spdlog::info ("Creating shader programs."); }

		{
			auto& shader = sceneTree.materials[0].program;
			SHADER::Create (shader, svfSimple, sffColorize);
			SHADER::CreateUniforms (shader, 1, uniNames, setFuncs);
		}

		{
			auto& shader = sceneTree.materials[1].program;
			SHADER::Create (shader, svfColorize, sffColorize);
			SHADER::CreateUniforms (shader, 1, uniNames, setFuncs);
		}

		DEBUG { spdlog::info ("Creating render meshes."); }

        { // STATIC Square MESH render.
        	auto& verticesSize = MESH::DD::SQUARE::VERTICES_COUNT;
        	auto& vertices = MESH::DD::SQUARE::VERTICES;
        	auto& indicesSize = MESH::DD::SQUARE::INDICES_COUNT;
        	auto& indices = MESH::DD::SQUARE::INDICES;
           //
            auto& mesh = sceneTree.meshes[0].base;
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
            auto& mesh = sceneTree.meshes[1].base;
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

        for (u64 i = 0; i < sceneTree.meshesCount; ++i) {
            auto& mesh = sceneTree.meshes[i].base;
            glDeleteVertexArrays (1, &mesh.vao);
            glDeleteBuffers (mesh.buffersCount, mesh.buffers);
        }

        delete[] sceneTree.meshes;


        DEBUG { spdlog::info ("Destroying shader programs."); }

		for (u64 i = 0; i < sceneTree.materialsCount; ++i) {
			auto& material = sceneTree.materials[i];
			SHADER::Destroy (material.program);
		}

		delete[] materialMeshes;
		delete[] sceneTree.materials;

	}


}