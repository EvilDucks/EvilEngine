#pragma once

#include "types.hpp"

#include "render/mesh.hpp"
#include "render/material.hpp"

namespace SCENES {

	//namespace RenderObject {
	//	// UI object that doesn't change its position, color, texture frequently.
	//	//  That information will be stored in less optimized gpu memory for such activity.
	//	struct Static {
	//		GLuint verticesId;
	//		GLuint programId;
	//		GLuint verticiesCount;
	//	}
	//}
	

	struct SceneTree {
        u64 meshesCount = 0;
        MESH::Base* meshes = nullptr;
		u64 materialsCount = 0;
		MATERIAL::Base* materials = nullptr;
	};

	// types
	//  - screen render object
	//  - projected render object

}

namespace GLOBAL {

	Color4 backgroundColor = Color4 ( 114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f );
	u16 windowSize[2] { 1200, 640 };


	#define D_SHADERS "res/shaders/"

	// Shader Vertex FilePath
	const char* svfTriangle = "res/shaders/Triangle.vert";
	const char* sffTriangle = "res/shaders/Triangle.frag";
	const char* sffRed = "res/shaders/Red.frag";

	// SET DURING INITIALIZATION
	SCENES::SceneTree sceneTree { 0 };






	void Initialize () {



		sceneTree.materialsCount = 2;
		sceneTree.materials = new MATERIAL::Base[sceneTree.materialsCount] { 0 };
        sceneTree.meshesCount = 2;
        sceneTree.meshes = new MESH::Base[sceneTree.meshesCount] { 0 };

		DEBUG { spdlog::info ("Creating shader programs."); }
		SHADER::Create (sceneTree.materials[0].program, svfTriangle, sffTriangle);
		SHADER::Create (sceneTree.materials[1].program, svfTriangle, sffRed);

		DEBUG { spdlog::info ("Creating render meshes."); }

        { // STATIC Square MESH render.
        	auto& verticesSize = MESH::DD::SQUARE::VERTICES_COUNT;
        	auto& vertices = MESH::DD::SQUARE::VERTICES;
        	auto& indicesSize = MESH::DD::SQUARE::INDICES_COUNT;
        	auto& indices = MESH::DD::SQUARE::INDICES;
           //
            auto& mesh = sceneTree.meshes[0];
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
            auto& mesh = sceneTree.meshes[1];
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
            auto& mesh = sceneTree.meshes[i];
            glDeleteVertexArrays (1, &mesh.vao);
            glDeleteBuffers (mesh.buffersCount, mesh.buffers);
        }

        delete[] sceneTree.meshes;


        DEBUG { spdlog::info ("Destroying shader programs."); }

		for (u64 i = 0; i < sceneTree.materialsCount; ++i) {
			auto& material = sceneTree.materials[i];
			glDeleteProgram (material.program);
		}

		delete[] sceneTree.materials;

	}


}


	// So what we can change
	// - We can specify how we want to treat the vertices array.
	// - The way we want to store our verticies.
	// - Create more/less VBO's -> AttribPointers (without EBO).
	// - We can use or not EBO.
	// - Knowing the number of meshes we can create more then 1 vao buffer.

	// What can I do with a VAO that has multiple VBO's?
	// Can I have a VAO with 1 VBO&EBO and 1 VBO only?
	// AttribPointer vs Uniform?
	//  -> Mesh is AttribPointer, Material is Uniform.
	//  -> Uniform value changes each render frame, AttribPointer is set once.


	//
	// struct Mesh {
	//     vao
	//     vbo (always at 0 index)
	//	   ebo (if exists always at 1 index)
	//	   buffors_count (if ebo at 1 or 2)
	//	   buffors (will use glVertexAttribPointer)
	// };
	//
	// void GetMeshBufforsCount() -> (vbo != nullptr) + (ebo != nullptr) + buffors_count;
	// void GetMeshLayoutsCount() -> (vbo != nullptr) + buffors_count;
	//

	//
	// struct Material {
	//     shader
	//     uniforms_count
	//     uniforms
	// }
	//
	// struct uniform to byłoby odwołanie do funckji która zawsze przyjmuje 5 argumentów z tym, że niekiedy
	//  argument n'ty może być nie wykorzystany, ustawiamy wtedy w zawołaniu wartość null lub podobną
	//  problem? -> To dość krytyczne miejsce na pointer_funkcji, dalakie odległości względem storny/stronami
	//  musze dobrze pomyśleć jak od strony silnika będzie wyglądać tworzenie materiałów, a ich późniejsze działanie
	//  bazujące na czytaniu wartości działań wcześniejszych wewnątrz funkcji Render.