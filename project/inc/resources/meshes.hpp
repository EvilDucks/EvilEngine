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
        sMeshesCount = 4;
		cMeshesCount = 0;
		wMeshesCount = 4;

        if (sMeshesCount) sMeshes = new MESH::Mesh[sMeshesCount] { 0 };
		if (cMeshesCount) cMeshes = new MESH::Mesh[cMeshesCount] { 0 };
		if (wMeshesCount) wMeshes = new MESH::Mesh[wMeshesCount] { 0 };
    }

    void CalculateMeshBounds (
            MESH::Mesh& mesh,
            u8 verticesCount,
            const float *vertices
    ) {
        glm::vec3 min = glm::vec3(0.f);
        glm::vec3 max = glm::vec3(0.f);
        for (int i = 0; i < verticesCount; i += 3)
        {
            min.x = std::min(float(vertices[i]), min.x);
            min.y = std::min(float(vertices[i+1]), min.y);
            min.z = std::min(float(vertices[i+2]), min.z);

            max.x = std::max(float(vertices[i]), max.x);
            max.y = std::max(float(vertices[i+1]), max.y);
            max.z = std::max(float(vertices[i+2]), max.z);
        }
        mesh.base.boundsMax = max;
        mesh.base.boundsMin = min;
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
                CalculateMeshBounds(wMeshes[0], MESH::DDD::CUBE::VERTICES_COUNT, MESH::DDD::CUBE::VERTICES);
			}

			//{ // STATIC Square MESH render.
			//	auto& verticesCount = MESH::DD::SQUARE::VERTICES_COUNT;
			//	auto& vertices = MESH::DD::SQUARE::VERTICES;
			//	auto& indicesCount = MESH::DD::SQUARE::INDICES_COUNT;
			//	auto& indices = MESH::DD::SQUARE::INDICES;
			//	//
			//	auto& componentMesh = wMeshes[1];
			//	auto& mesh = componentMesh.base;
			//	//
			//	MESH::VI::CreateVAO (
			//		mesh.vao, mesh.buffers,
			//		verticesCount, vertices,
			//		indicesCount, indices
			//	);
			//	//
			//	mesh.verticiesCount = indicesCount;
			//	mesh.drawFunc = MESH::VI::Draw;
			//	componentMesh.id = OBJECT::_4;
			//}

			
			{ // STATIC Square MESH render.
				auto& verticesCount = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& vertices = MESH::DD::SQUARE::VERTICES_UV;
				auto& indicesCount = MESH::DD::SQUARE::INDICES_COUNT;
				auto& indices = MESH::DD::SQUARE::INDICES;
				//
				auto& componentMesh = wMeshes[1];
				auto& mesh = componentMesh.base;
				//
				MESH::VIT::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesCount, vertices,
					indicesCount, indices
				);
				//
				mesh.verticiesCount = indicesCount;
				mesh.drawFunc = MESH::VIT::Draw;
				componentMesh.id = OBJECT::_4;
                CalculateMeshBounds(wMeshes[1], MESH::DD::SQUARE::VERTICES_COUNT, MESH::DD::SQUARE::VERTICES);
			}

            { // Temporary cube player MESH render.
                auto& verticesCount = MESH::DDD::CUBE::VERTICES_COUNT;
                auto& vertices = MESH::DDD::CUBE::VERTICES;
                //
                auto& componentMesh = wMeshes[2];
                auto& mesh = componentMesh.base;
                //
                MESH::V::CreateVAO (
                        mesh.vao, mesh.buffers,
                        verticesCount, vertices
                );
                //
                mesh.verticiesCount = verticesCount;
                mesh.drawFunc = MESH::V::Draw;
                componentMesh.id = OBJECT::_player;
                CalculateMeshBounds(wMeshes[2], MESH::DDD::CUBE::VERTICES_COUNT, MESH::DDD::CUBE::VERTICES);
            }

            { // STATIC wall MESH render.
                auto& verticesCount = MESH::DDD::CUBE::VERTICES_COUNT;
                auto& vertices = MESH::DDD::CUBE::VERTICES;
                //
                auto& componentMesh = wMeshes[3];
                auto& mesh = componentMesh.base;
                //
                MESH::V::CreateVAO (
                        mesh.vao, mesh.buffers,
                        verticesCount, vertices
                );
                //
                mesh.verticiesCount = verticesCount;
                mesh.drawFunc = MESH::V::Draw;
                componentMesh.id = OBJECT::_testWall;
                CalculateMeshBounds(wMeshes[3], MESH::DDD::CUBE::VERTICES_COUNT, MESH::DDD::CUBE::VERTICES);
            }
		}

		{ // CANVAS

		}

		{ // Screen

			//{ // STATIC Square MESH render.
			//	auto& verticesCount = MESH::DD::FULL_SQUARE::VERTICES_COUNT;
			//	auto& vertices = MESH::DD::FULL_SQUARE::VERTICES;
			//	auto& indicesCount = MESH::DD::FULL_SQUARE::INDICES_COUNT;
			//	auto& indices = MESH::DD::FULL_SQUARE::INDICES;
			//	//
			//	auto& componentMesh = sMeshes[0];
			//	auto& mesh = componentMesh.base;
			//	//
			//	MESH::VI::CreateVAO (
			//		mesh.vao, mesh.buffers,
			//		verticesCount, vertices,
			//		indicesCount, indices
			//	);
			//	//
			//	mesh.verticiesCount = indicesCount;
			//	mesh.drawFunc = MESH::VI::Draw;
			//	componentMesh.id = OBJECT::_1;
			//}

			{ // STATIC SCREEN FULFILL MESH render.
				auto& verticesCount = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& vertices = MESH::DD::FULL_SQUARE::VERTICES_UV;
				auto& indicesCount = MESH::DD::SQUARE::INDICES_COUNT;
				auto& indices = MESH::DD::SQUARE::INDICES;
				//
				auto& componentMesh = sMeshes[0];
				auto& mesh = componentMesh.base;
				//
				MESH::VIT::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesCount, vertices,
					indicesCount, indices
				);
				//
				mesh.verticiesCount = indicesCount;
				mesh.drawFunc = MESH::VIT::Draw;
				componentMesh.id = OBJECT::_1;
                CalculateMeshBounds(sMeshes[0], MESH::DD::SQUARE::VERTICES_COUNT, MESH::DD::SQUARE::VERTICES);
			}

			{ // SCREEN SMALL SQUARE 1
				auto& verticesCount = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& vertices = MESH::DD::SAMPLE1_SQUARE::VERTICES_UV;
				auto& indicesCount = MESH::DD::SQUARE::INDICES_COUNT;
				auto& indices = MESH::DD::SQUARE::INDICES;
				//
				auto& componentMesh = sMeshes[1];
				auto& mesh = componentMesh.base;
				//
				MESH::VIT::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesCount, vertices,
					indicesCount, indices
				);
				//
				mesh.verticiesCount = indicesCount;
				mesh.drawFunc = MESH::VIT::Draw;
				componentMesh.id = OBJECT::_7_SQUARE_1;
			}

			{ // SCREEN SMALL SQUARE 2
				auto& verticesCount = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& vertices = MESH::DD::SAMPLE2_SQUARE::VERTICES_UV;
				auto& indicesCount = MESH::DD::SQUARE::INDICES_COUNT;
				auto& indices = MESH::DD::SQUARE::INDICES;
				//
				auto& componentMesh = sMeshes[2];
				auto& mesh = componentMesh.base;
				//
				MESH::VIT::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesCount, vertices,
					indicesCount, indices
				);
				//
				mesh.verticiesCount = indicesCount;
				mesh.drawFunc = MESH::VIT::Draw;
				componentMesh.id = OBJECT::_8_SQUARE_2;
			}

			{ // STATIC Triangle MESH render.
				auto& verticesCount = MESH::DD::TRIANGLE::VERTICES_COUNT;
				auto& vertices = MESH::DD::TRIANGLE::VERTICES;
				//
				auto& componentMesh = sMeshes[3];
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
                CalculateMeshBounds(sMeshes[1], MESH::DD::TRIANGLE::VERTICES_COUNT, MESH::DD::TRIANGLE::VERTICES);
			}

		}
        
    }

}