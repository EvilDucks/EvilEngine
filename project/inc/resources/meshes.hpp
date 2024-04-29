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
        ZoneScopedN("RESOURCES::MESHES: DeleteMeshes");

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
        ZoneScopedN("RESOURCES::MESHES: CreateMeshes");

        sMeshesCount = 4;
		cMeshesCount = 0;
		wMeshesCount = 6;

        if (sMeshesCount) sMeshes = new MESH::Mesh[sMeshesCount] { 0 };
		if (cMeshesCount) cMeshes = new MESH::Mesh[cMeshesCount] { 0 };
		if (wMeshesCount) wMeshes = new MESH::Mesh[wMeshesCount] { 0 };
    }

    void CalculateMeshBounds (
            MESH::Mesh& mesh,
            u8 verticesCount,
            const float *vertices
    ) {
        ZoneScopedN("RESOURCES::MESHES: CalculateMeshBounds");

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

        mesh.base.boundsRadius = std::max(std::max( std::abs(min.x - max.x), std::abs(min.y - max.y)), std::abs(min.z - max.z)) * 0.5f;
    }

    void LoadMeshes (
		/* IN  */ Json& meshesJson,
		/* OUT */ u64& sMeshesCount, 
        /* OUT */ MESH::Mesh*& sMeshes,
		/* OUT */ u64& cMeshesCount,
        /* OUT */ MESH::Mesh*& cMeshes,
		/* OUT */ u64& wMeshesCount, 
        /* OUT */ MESH::Mesh*& wMeshes,
		/* OUT */ MESH::Mesh& skyboxMesh
	) {
        ZoneScopedN("RESOURCES::MESHES: LoadMeshes");

		{ // SKYBOX
			auto& verticesCount = MESH::DDD::SKYBOX::VERTICES_COUNT;
            auto& vertices = MESH::DDD::SKYBOX::VERTICES;
			auto& componentMesh = skyboxMesh;
            auto& mesh = componentMesh.base;
			//
			MESH::V::CreateVAO (
                    mesh.vao, mesh.buffers,
                    verticesCount, vertices
            );
			//
			mesh.verticiesCount = verticesCount;
            mesh.drawFunc = MESH::V::Draw;
            componentMesh.id = OBJECT::_11_SKYBOX;
		}

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
				componentMesh.id = OBJECT::_03;
                CalculateMeshBounds(componentMesh, MESH::DDD::CUBE::VERTICES_COUNT, MESH::DDD::CUBE::VERTICES);
			}

			
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
				componentMesh.id = OBJECT::_04;
				//
                CalculateMeshBounds(componentMesh, MESH::DD::SQUARE::VERTICES_COUNT, MESH::DD::SQUARE::VERTICES);
			}

			//{
			//	std::vector<GLfloat> vertices;
			//	std::vector<GLuint> indices;
			//	std::vector<GLfloat> normals;
			//	std::vector<GLfloat> texCoords;
			//	//
			//	vertices.clear();
			//	indices.clear();
			//	normals.clear();
			//	texCoords.clear();
			//	//
			//	MESH::DDD::DSPHERE::CreateVertices (vertices, indices, normals, texCoords, 4, 4, 1.0f);
			//	//
			//	spdlog::info ("{0}, {1}, {2}, {3}", vertices.size(), indices.size(), normals.size(), texCoords.size());
			//	//
			//	//Sphere sphere (1.0f, 36, 18);
			//	Sphere sphere (1.0f, 4, 4);
			//	//
			//	spdlog::info ("{0}, {1}, {2}, {3}", 
			//		sphere.getVertexCount(), 
			//		sphere.getIndexCount(), 
			//		sphere.getNormalCount(), 
			//		sphere.getTexCoordCount()
			//	);
			//	//
			//	spdlog::info ("{0}, {1}, {2}, {3}", 
			//		sphere.vertices.size(), 
			//		sphere.indices.size(), 
			//		sphere.normals.size(), 
			//		sphere.texCoords.size()
			//	);
			//	//
			//	for (u16 i = 0; i < vertices.size(); ++i) {
			//		if (vertices[i] != sphere.vertices[i]) {
			//			spdlog::info ("vi: {0}, a: {1}, b: {2}", i, vertices[i], sphere.vertices[i]);
			//		}
			//	}
			//	for (u16 i = 0; i < indices.size(); ++i) {
			//		if (indices[i] != sphere.indices[i]) {
			//			spdlog::info ("ii: {0}, a: {1}, b: {2}", i, indices[i], sphere.indices[i]);
			//		}
			//	}
			//	//
			//	//exit(1);
			//	auto& componentMesh = wMeshes[2];
			//	auto& mesh = componentMesh.base;
			//	//
			//	//MESH::VIT::CreateVAO (
			//	//	mesh.vao, mesh.buffers,
			//	//	vertices.size() / 3, vertices.data (),
			//	//	indices.size(), indices.data ()
			//	//);
			//	//mesh.verticiesCount = indices.size();
			//	MESH::VIT::CreateVAO (
			//		mesh.vao, mesh.buffers,
			//		sphere.getVertexCount(), sphere.vertices.data (),
			//		sphere.getIndexCount(), sphere.indices.data ()
			//	);
			//	mesh.verticiesCount = sphere.getIndexCount();
			//	//
			//	mesh.drawFunc = MESH::VIT::Draw;
			//	componentMesh.id = OBJECT::_07_player;
			//	//
            //    CalculateMeshBounds (componentMesh, MESH::DD::SQUARE::VERTICES_COUNT, MESH::DD::SQUARE::VERTICES);
			//}

			{
				u16 verticesCount;
				GLfloat* vertices;
				MESH::DDD::DSPHERE::CreateVertices (verticesCount, vertices, 4, 4, 1.0f); // Sectors, stacks, radius.
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
				componentMesh.id = OBJECT::_07_player;
                CalculateMeshBounds (componentMesh, MESH::DDD::CUBE::VERTICES_COUNT, MESH::DDD::CUBE::VERTICES);
				
				MESH::DDD::DSPHERE::DestroyVertices (vertices);
			}

			//{ // Temporary cube player MESH render.
			//	auto& cubeMesh = wMeshes[0]; // COPY exsisting cube instead
			//	auto& componentMesh = wMeshes[2];
			//	componentMesh = cubeMesh; // CPY
			//	componentMesh.id = OBJECT::_07_player;
			//}

            { // STATIC wall MESH render.
				auto& cubeMesh = wMeshes[0]; // COPY exsisting cube instead
				auto& componentMesh = wMeshes[3];
				
				componentMesh = cubeMesh; // CPY
                componentMesh.id = OBJECT::_08_testWall;
            }

			{ // Ground
				auto& planeMesh = wMeshes[1]; // COPY exsisting cube instead
				auto& componentMesh = wMeshes[4];

				componentMesh = planeMesh; // CPY
				componentMesh.id = OBJECT::_12_GROUND;
			}

			{ // Ground
				auto& cubeMesh = wMeshes[0]; // COPY exsisting cube instead
				auto& componentMesh = wMeshes[5];

				componentMesh = cubeMesh; // CPY
				componentMesh.id = OBJECT::_13_LIGHT_1;
			}

		}

		{ // CANVAS

		}

		{ // Screen

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
				componentMesh.id = OBJECT::_01;
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
				componentMesh.id = OBJECT::_09_SQUARE_1;
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
				componentMesh.id = OBJECT::_10_SQUARE_2;
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
				componentMesh.id = OBJECT::_02;
                CalculateMeshBounds(sMeshes[1], MESH::DD::TRIANGLE::VERTICES_COUNT, MESH::DD::TRIANGLE::VERTICES);
			}

		}
        
    }

}