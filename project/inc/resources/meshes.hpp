#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

// For now only.
#include "object.hpp"

namespace RESOURCES::MESHES {

    void DeleteMeshes (
        /* IN  */ u64& sMeshesCount, 
        /* IN  */ MESH::Mesh*& sMeshes,
		/* IN  */ u64& cMeshesCount, 
        /* IN  */ MESH::Mesh*& cMeshes,
		/* IN  */ u64& wMeshesCount, 
        /* IN  */ MESH::Mesh*& wMeshes
    ) {

        for (u64 i = 0; i < sMeshesCount; ++i) {
			auto& mesh = sMeshes[i].base;
			glDeleteVertexArrays (1, &mesh.vao);
			glDeleteBuffers (mesh.buffersCount, mesh.buffers);
		}

		delete[] sMeshes;

		for (u64 i = 0; i < cMeshesCount; ++i) {
			auto& mesh = cMeshes[i].base;
			glDeleteVertexArrays (1, &mesh.vao);
			glDeleteBuffers (mesh.buffersCount, mesh.buffers);
		}

		delete[] cMeshes;

		for (u64 i = 0; i < wMeshesCount; ++i) {
			auto& mesh = wMeshes[i].base;
			glDeleteVertexArrays (1, &mesh.vao);
			glDeleteBuffers (mesh.buffersCount, mesh.buffers);
		}

		delete[] wMeshes;

    }


    void CreateMeshes (
		/* OUT */ Json& meshesJson,
		/* OUT */ u64& sMeshesCount, 
        /* OUT */ MESH::Mesh*& sMeshes,
		/* OUT */ u64& cMeshesCount, 
        /* OUT */ MESH::Mesh*& cMeshes,
		/* OUT */ u64& wMeshesCount, 
        /* OUT */ MESH::Mesh*& wMeshes
	) {
        sMeshesCount = 2;
		cMeshesCount = 0;
		wMeshesCount = 2;

        if (sMeshesCount) sMeshes = new MESH::Mesh[sMeshesCount] { 0 };
		if (cMeshesCount) cMeshes = new MESH::Mesh[cMeshesCount] { 0 };
		if (wMeshesCount) wMeshes = new MESH::Mesh[wMeshesCount] { 0 };
    }


    void LoadMeshes (
		/* IN  */ Json& meshesJson,
		/* OUT */ u64& sMeshesCount, 
        /* OUT */ MESH::Mesh*& sMeshes,
		/* OUT */ u64& cMeshesCount, 
        /* OUT */ MESH::Mesh*& cMeshes,
		/* OUT */ u64& wMeshesCount, 
        /* OUT */ MESH::Mesh*& wMeshes
	) {

        { // WORLD

			{ // STATIC Cube MESH render.
				auto& verticesCount = MESH::DDD::CUBE::VERTICES_COUNT;
				auto& vertices = MESH::DDD::CUBE::VERTICES;
				//
				auto& componentMesh = wMeshes[0];
				auto& mesh = componentMesh.base;
				//
				MESH::V::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesCount, vertices
				);
				//
				mesh.verticiesCount = verticesCount;
				mesh.drawFunc = MESH::V::Draw;
				componentMesh.id = OBJECT::_3;
			}

			{ // STATIC Square MESH render.
				auto& verticesCount = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& vertices = MESH::DD::SQUARE::VERTICES;
				auto& indicesCount = MESH::DD::SQUARE::INDICES_COUNT;
				auto& indices = MESH::DD::SQUARE::INDICES;
				//
				auto& componentMesh = wMeshes[1];
				auto& mesh = componentMesh.base;
				//
				MESH::VI::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesCount, vertices,
					indicesCount, indices
				);
				//
				mesh.verticiesCount = indicesCount;
				mesh.drawFunc = MESH::VI::Draw;
				componentMesh.id = OBJECT::_4;
			}

		}

		{ // CANVAS

		}

		{ // Screen

			{ // STATIC Square MESH render.
				auto& verticesCount = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& vertices = MESH::DD::SQUARE::VERTICES;
				auto& indicesCount = MESH::DD::SQUARE::INDICES_COUNT;
				auto& indices = MESH::DD::SQUARE::INDICES;
				//
				auto& componentMesh = sMeshes[0];
				auto& mesh = componentMesh.base;
				//
				MESH::VI::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesCount, vertices,
					indicesCount, indices
				);
				//
				mesh.verticiesCount = indicesCount;
				mesh.drawFunc = MESH::VI::Draw;
				componentMesh.id = OBJECT::_1;
			}

			{ // STATIC Triangle MESH render.
				auto& verticesCount = MESH::DD::TRIANGLE::VERTICES_COUNT;
				auto& vertices = MESH::DD::TRIANGLE::VERTICES;
				//
				auto& componentMesh = sMeshes[1];
				auto& mesh = componentMesh.base;
				//
				MESH::V::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesCount, vertices
				);
				//
				mesh.verticiesCount = verticesCount;
				mesh.drawFunc = MESH::V::Draw;
				componentMesh.id = OBJECT::_2;
			}

		}
        
    }

}