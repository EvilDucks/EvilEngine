#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

// For now only.
#include "object.hpp"
#include "render/temp/Spheres.hpp"
#include "render/temp/Cubesphere.h"
#include "render/temp/Icosahedron.h"
#include "render/temp/Icosphere.h"

namespace RESOURCES::MESHES {

    void DeleteMeshes (
        /* IN  */ u8& sMeshesCount, 
        /* IN  */ MESH::Mesh*& sMeshes,
		/* IN  */ u8& cMeshesCount, 
        /* IN  */ MESH::Mesh*& cMeshes,
		/* IN  */ u8& wMeshesCount, 
        /* IN  */ MESH::Mesh*& wMeshes
    ) {
        PROFILER { ZoneScopedN("RESOURCES::MESHES: DeleteMeshes"); }

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
		/* OUT */ u8& sMeshesCount, 
        /* OUT */ MESH::Mesh*& sMeshes,
		/* OUT */ u8& cMeshesCount, 
        /* OUT */ MESH::Mesh*& cMeshes,
		/* OUT */ u8& wMeshesCount, 
        /* OUT */ MESH::Mesh*& wMeshes
	) {
        PROFILER { ZoneScopedN("RESOURCES::MESHES: CreateMeshes"); }

        sMeshesCount = 4;
		cMeshesCount = 1;
		wMeshesCount = 3;

        if (sMeshesCount) sMeshes = new MESH::Mesh[sMeshesCount] { 0 };
		if (cMeshesCount) cMeshes = new MESH::Mesh[cMeshesCount] { 0 };
		if (wMeshesCount) wMeshes = new MESH::Mesh[wMeshesCount] { 0 };
    }

    void CalculateMeshBounds (
            MESH::Mesh& mesh,
            u8 verticesCount,
            const float *vertices
    ) {
        PROFILER { ZoneScopedN("RESOURCES::MESHES: CalculateMeshBounds"); }

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

        mesh.base.boundsRadius = std::max(
			std::max( 
				std::abs(min.x - max.x), 
				std::abs(min.y - max.y)
			), 
			std::abs(min.z - max.z)
		) * 0.5f;
    }

    void LoadMeshes (
		/* IN  */ Json& meshesJson,

		/* OUT */ u8& sMeshesCount, 
        /* OUT */ MESH::Mesh*& sMeshes,
		/* IN  */ u8*&  sInstancesCounts,

		/* OUT */ u8& cMeshesCount,
        /* OUT */ MESH::Mesh*& cMeshes,
		/* IN  */ u8*&  cInstancesCounts,

		/* OUT */ u8& wMeshesCount, 
        /* OUT */ MESH::Mesh*& wMeshes,
		/* IN  */ u8*&  wInstancesCounts,

		/* OUT */ MESH::Mesh& skyboxMesh
	) {
        PROFILER { ZoneScopedN("RESOURCES::MESHES: LoadMeshes"); }

		// SPHERES
		Sphere sphere (0.5f, 36, 18, true, 3);
		//
		Icosahedron icosahedron (1.0f);
		//
		Icosphere icosphere (0.5f, 5, true);
		//icosphere.setRadius (2.0f);
		icosphere.setSubdivision (4);
		icosphere.setSmooth (false);
		//
		Cubesphere cubesphere (0.5, 3, true);
		//cubesphere.setRadius (2.0f);
		cubesphere.setSubdivision (2);
		cubesphere.setSmooth (false);

		u16 tVerticesCount;
		GLfloat* tVertices;
		u16 tIndicesCount;
		GLuint* tIndices;

		{ // Creation of dynamic shapes
			// Sectors, radius.
			// MESH::DD::DCIRCLE::CreateVertices (verticesCount, vertices, 8, 1.0f); // V
			// MESH::DD::DCIRCLE::CreateVertices (verticesCount, vertices, indicesCount, indices, 8, 1.0); // VI
			// Sectors, length, radius.
			// MESH::DDD::DCONE::CreateVertices (verticesCount, vertices, indicesCount, indices, 3, 1.0, 0.5); // VI
			MESH::DDD::DCYLINDER::CreateVertices (tVerticesCount, tVertices, tIndicesCount, tIndices, 18, 1.0, 0.5); // VI
		}

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
            componentMesh.id = 0;
		}

        { // WORLD

			{ // STATIC Cube MESH render.
				auto& verticesCount = MESH::DDD::CUBE::VERTICES_COUNT;
				auto& vertices = MESH::DDD::CUBE::VERTICES;
				//
				auto meshId = 0;
				auto& componentMesh = wMeshes[meshId];
				auto& mesh = componentMesh.base;
				//
				MESH::INSTANCED::V::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesCount, vertices,
					wInstancesCounts[meshId]
				);
				//
				mesh.verticiesCount = verticesCount;
				mesh.drawFunc = MESH::INSTANCED::V::Draw;
				componentMesh.id = 0;
                CalculateMeshBounds (componentMesh, MESH::DDD::CUBE::VERTICES_COUNT, MESH::DDD::CUBE::VERTICES);
			}

			
			{ // STATIC Square MESH render.
				auto& verticesCount = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& vertices = MESH::DD::SQUARE::VERTICES;
				auto& indicesCount = MESH::DD::SQUARE::INDICES_COUNT;
				auto& indices = MESH::DD::SQUARE::INDICES;
				auto& uvsCount = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& uvs = MESH::DD::SQUARE::UVS;
				//
				auto meshId = 1;
				auto& componentMesh = wMeshes[meshId];
				auto& mesh = componentMesh.base;
				//
				MESH::INSTANCED::XVIT::CreateVAO (
					mesh.vao, mesh.buffers,
					verticesCount, vertices,
					indicesCount, indices,
					uvsCount, uvs,
					wInstancesCounts[meshId]
				);
				//
				mesh.verticiesCount = indicesCount;
				mesh.drawFunc = MESH::INSTANCED::XVIT::Draw;
				componentMesh.id = 0;
				//
                CalculateMeshBounds (componentMesh, MESH::DD::SQUARE::VERTICES_COUNT, MESH::DD::SQUARE::VERTICES);
			}

			//{ // SPHERE
			//	std::vector<GLfloat> vertices;
			//	std::vector<GLuint> indices;
			//	std::vector<GLfloat> normals;
			//	std::vector<GLfloat> texCoords;
			//	//
			//	MESH::DDD::DSPHERE::CreateVerticesOld (vertices, indices, normals, texCoords, 12, 12, 0.5f);
			//	//
			//	//spdlog::info ("{0}, {1}, {2}, {3}", vertices.size(), indices.size(), normals.size(), texCoords.size());
			//	////
			//	////Sphere sphere (1.0f, 36, 18);
			//	//Sphere sphere (1.0f, 18, 18);
			//	//
			//	//spdlog::info ("{0}, {1}, {2}, {3}", 
			//	//	sphere.getVertexCount(), 
			//	//	sphere.getIndexCount(), 
			//	//	sphere.getNormalCount(), 
			//	//	sphere.getTexCoordCount()
			//	//);
			//	////
			//	//spdlog::info ("{0}, {1}, {2}, {3}", 
			//	//	sphere.vertices.size(), 
			//	//	sphere.indices.size(), 
			//	//	sphere.normals.size(), 
			//	//	sphere.texCoords.size()
			//	//);
			//	//
			//	//for (u16 i = 0; i < vertices.size(); ++i) {
			//	//	if (vertices[i] != sphere.vertices[i]) {
			//	//		spdlog::info ("vi: {0}, a: {1}, b: {2}", i, vertices[i], sphere.vertices[i]);
			//	//	}
			//	//}
			//	//for (u16 i = 0; i < indices.size(); ++i) {
			//	//	if (indices[i] != sphere.indices[i]) {
			//	//		spdlog::info ("ii: {0}, a: {1}, b: {2}", i, indices[i], sphere.indices[i]);
			//	//	}
			//	//}
			//	//
			//	//exit(1);
			//	auto& componentMesh = wMeshes[2];
			//	auto& mesh = componentMesh.base;
			//	//
			//	MESH::VI::CreateVAO (
			//		mesh.vao, mesh.buffers,
			//		vertices.size() / 3, vertices.data (),
			//		indices.size(), indices.data ()
			//	);
			//	mesh.verticiesCount = indices.size();
			//	//MESH::VI::CreateVAO (
			//	//	mesh.vao, mesh.buffers,
			//	//	sphere.getVertexCount(), sphere.vertices.data (),
			//	//	sphere.getIndexCount(), sphere.indices.data ()
			//	//);
			//	//mesh.verticiesCount = sphere.getIndexCount();
			//	//
			//	mesh.drawFunc = MESH::VI::Draw;
			//	componentMesh.id = OBJECT::_07_player;
			//	//
            //    CalculateMeshBounds (componentMesh, MESH::DD::SQUARE::VERTICES_COUNT, MESH::DD::SQUARE::VERTICES);
			//}

			//{ // CYLINDER EXAMPLE (IDICES)
			//	auto& componentMesh = wMeshes[2];
			//	auto& mesh = componentMesh.base;
			//	//
			//	MESH::VI::CreateVAO (
			//		mesh.vao, mesh.buffers,
			//		tVerticesCount, tVertices,
			//		tIndicesCount, tIndices
			//	);
			//	//
			//	mesh.verticiesCount = tIndicesCount;
			//	mesh.drawFunc = MESH::VI::Draw;
			//	componentMesh.id = OBJECT::_07_player;
            //    CalculateMeshBounds (componentMesh, MESH::DDD::CUBE::VERTICES_COUNT, MESH::DDD::CUBE::VERTICES);
			//}

			//{ // ICOSAHEDRON
			//	auto& componentMesh = wMeshes[2];
			//	auto& mesh = componentMesh.base;
			//	//
			//	MESH::VI::CreateVAO (
			//		mesh.vao, mesh.buffers,
			//		icosahedron.getVertexCount(), icosahedron.vertices.data(),
			//		icosahedron.indices.size(), icosahedron.indices.data()
			//	);
			//	//
			//	mesh.verticiesCount = icosahedron.indices.size ();
			//	mesh.drawFunc = MESH::VI::Draw;
			//	componentMesh.id = OBJECT::_07_player;
            //    CalculateMeshBounds (componentMesh, MESH::DDD::CUBE::VERTICES_COUNT, MESH::DDD::CUBE::VERTICES);
			//}

			//{ // ICOSPHERE
			//	auto& componentMesh = wMeshes[2];
			//	auto& mesh = componentMesh.base;
			//	//
			//	MESH::VI::CreateVAO (
			//		mesh.vao, mesh.buffers,
			//		icosphere.getVertexCount(), icosphere.vertices.data(),
			//		icosphere.indices.size(), icosphere.indices.data()
			//	);
			//	//
			//	mesh.verticiesCount = icosphere.indices.size ();
			//	mesh.drawFunc = MESH::VI::Draw;
			//	componentMesh.id = OBJECT::_07_player;
            //    CalculateMeshBounds (componentMesh, MESH::DDD::CUBE::VERTICES_COUNT, MESH::DDD::CUBE::VERTICES);
			//}

			//{ // CUBESPHERE
			//	auto meshId = 2;
			//	auto& componentMesh = wMeshes[meshId];
			//	auto& mesh = componentMesh.base;
			//	//
			//	MESH::INSTANCED::VI::CreateVAO (
			//		mesh.vao, mesh.buffers,
			//		cubesphere.getVertexCount(), cubesphere.vertices.data(),
			//		cubesphere.indices.size(), cubesphere.indices.data(),
			//		wInstancesCounts[meshId]
			//	);
			//	//
			//	mesh.verticiesCount = cubesphere.indices.size ();
			//	mesh.drawFunc = MESH::INSTANCED::VI::Draw;
			//	componentMesh.id = 0;
            //    CalculateMeshBounds (componentMesh, MESH::DDD::CUBE::VERTICES_COUNT, MESH::DDD::CUBE::VERTICES);
			//}

			{ // SPHERE
				auto meshId = 2;
				auto& componentMesh = wMeshes[meshId];
				auto& mesh = componentMesh.base;
				//
				MESH::INSTANCED::XVITN::CreateVAO (
					mesh.vao, mesh.buffers,
					sphere.getVertexCount(), sphere.vertices.data(),
					sphere.indices.size(), sphere.indices.data(),
					sphere.texCoords.size() / 2, sphere.texCoords.data(),
					sphere.normals.size() / 3, sphere.normals.data(),
					wInstancesCounts[meshId]
				);
				//
				mesh.verticiesCount = sphere.indices.size ();
				mesh.drawFunc = MESH::INSTANCED::XVITN::Draw;
				componentMesh.id = 0;
                CalculateMeshBounds (componentMesh, MESH::DDD::CUBE::VERTICES_COUNT, MESH::DDD::CUBE::VERTICES);
			}

			//{ // Temporary cube player MESH render.
			//	auto& cubeMesh = wMeshes[0]; // COPY exsisting cube instead
			//	auto& componentMesh = wMeshes[2];
			//	componentMesh = cubeMesh; // CPY
			//	componentMesh.id = OBJECT::_07_player;
			//}

            //{ // STATIC wall MESH render.
			//	auto& cubeMesh = wMeshes[0]; // COPY exsisting cube instead
			//	auto& componentMesh = wMeshes[3];
			//	
			//	componentMesh = cubeMesh; // CPY
            //    componentMesh.id = OBJECT::_08_testWall;
            //}

			//{ // Ground
			//	auto& planeMesh = wMeshes[1]; // COPY exsisting square instead
			//	auto& componentMesh = wMeshes[4];
			//
			//	componentMesh = planeMesh; // CPY
			//	componentMesh.id = OBJECT::_12_GROUND;
			//}

			//{ // Light
			//	auto& cubeMesh = wMeshes[0]; // COPY exsisting cube instead
			//	auto& componentMesh = wMeshes[4];
			//
			//	componentMesh = cubeMesh; // CPY
			//	componentMesh.id = OBJECT::_13_LIGHT_1;
			//}

			{ // Deallocation of dynamic shapes.
				delete[] tVertices;
				delete[] tIndices;
			}

		}

		{ // CANVAS

			{ // SCREEN SMALL SQUARE 1
				auto& verticesCount = MESH::DD::SQUARE::VERTICES_COUNT;
				auto& vertices = MESH::DD::SQUARE::VERTICES_UV;
				auto& indicesCount = MESH::DD::SQUARE::INDICES_COUNT;
				auto& indices = MESH::DD::SQUARE::INDICES;
				//
				auto& componentMesh = cMeshes[0];
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
				componentMesh.id = 0;
			}

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
                //CalculateMeshBounds(sMeshes[0], MESH::DD::SQUARE::VERTICES_COUNT, MESH::DD::SQUARE::VERTICES);
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
                //CalculateMeshBounds(sMeshes[1], MESH::DD::TRIANGLE::VERTICES_COUNT, MESH::DD::TRIANGLE::VERTICES);
			}

		}
        
    }

}