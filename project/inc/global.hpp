#pragma once

#include "types.hpp"

#if PLATFORM == PLATFORM_WINDOWS
#include "platform/win/types.hpp"
#else
#include "platform/agn/types.hpp"
#endif

#include "components/parentship.hpp"
#include "components/mesh.hpp"
#include "components/transform.hpp"

#include "render/material.hpp"



namespace SCENE {

	struct Canvas {
		/* OTHER */
		u64 materialsCount = 0;
		MATERIAL::Material* materials = nullptr;
		/* COMPONENTS */
		u64 parentshipsCount = 0;
		PARENTSHIP::Parentship* parentships = nullptr;
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
		u64 parentshipsCount = 0;
		PARENTSHIP::Parentship* parentships = nullptr;
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


	Range<s64>* children;
	Range<s64>* entities;

	// Transfrom transfrom = entity[10].GetComponent<Transform>;
	// 

	
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
	const u64 mat1USize = 1;
	SHADER::UNIFORM::Uniform mat1Uniforms[] { color };
	const char* mat1UNames[] { unColor };


	const char unModel[] 		{ "model" };
	const char unView[] 		{ "view" };
	const char unProjection[] 	{ "projection" };

	// Unique buffers
	SHADER::UNIFORM::M4 ubProjection = glm::mat4(1.0f);
	SHADER::UNIFORM::M4 ubView = glm::mat4(1.0f);
	SHADER::UNIFORM::M4 ubGlobalSpace = glm::mat4(1.0f);

	SHADER::UNIFORM::Uniform projection { 0, &ubProjection, SHADER::UNIFORM::SetM4 };
	SHADER::UNIFORM::Uniform view { 0, &ubView, SHADER::UNIFORM::SetM4 };
	SHADER::UNIFORM::Uniform model { 0, &ubGlobalSpace, SHADER::UNIFORM::SetM4 };

	const u64 mat2USize = 3;
	SHADER::UNIFORM::Uniform mat2Uniforms[] { model, view, projection };
	const char* mat2UNames[] { unModel, unView, unProjection };

	// }


	void Initialize () {
		
		// It's all Data Layer, Memory allocations, Pointer assignments.

		const GameObjectID ENTITY_1 = 1;
		const GameObjectID ENTITY_2 = 2;
		const GameObjectID ENTITY_3 = 3;
		const GameObjectID ENTITY_4 = 4;

		{
			canvas.materialsCount = 2;
			canvas.meshesCount = 2;
			canvas.transformsCount = 2;
			canvas.parentshipsCount = 0;
		}

		{
			world.materialsCount = 1;
			world.meshesCount = 1;
			world.transformsCount = 2;
			world.parentshipsCount = 1;
		}

		// When reading json file we also get an array of how many each parentship has children.
		const u64 CANVAS_PARENTSHIPS_CHILDREN_COUNT[0]	{	};
		const u64 WORLD_PARENTSHIPS_CHILDREN_COUNT[1]	{ 1 };

		DEBUG { spdlog::info ("Allocating memory for collections & components."); }

		{
			// Collections
			if (canvas.materialsCount)
				canvas.materials = new MATERIAL::Material[canvas.materialsCount];
			// Components
			if (canvas.parentshipsCount)
				canvas.parentships = new PARENTSHIP::Parentship[canvas.parentshipsCount] { 0 };
			if (canvas.meshesCount)
				canvas.meshes = new MESH::Mesh[canvas.meshesCount] { 0 };
			if (canvas.transformsCount)
				canvas.transforms = new TRANSFORM::Transform[canvas.transformsCount] { 0 };
		}

		for (u64 i = 0; i < canvas.parentshipsCount; ++i) {
			auto& parentship = canvas.parentships[i].base;
			parentship.childrenCount = CANVAS_PARENTSHIPS_CHILDREN_COUNT[i];
			parentship.children = new GameObjectID[parentship.childrenCount] { 0 };
		}

		{
			// Collections
			if (world.materialsCount)
				world.materials = new MATERIAL::Material[world.materialsCount];
			// Components
			if (world.parentshipsCount)
				world.parentships = new PARENTSHIP::Parentship[world.parentshipsCount] { 0 };
			if (world.meshesCount)
				world.meshes = new MESH::Mesh[world.meshesCount] { 0 };
			if (world.transformsCount)
				world.transforms = new TRANSFORM::Transform[world.transformsCount] { 0 };
		}

		for (u64 i = 0; i < world.parentshipsCount; ++i) {
			auto& parentship = world.parentships[i].base;
			parentship.childrenCount = WORLD_PARENTSHIPS_CHILDREN_COUNT[i];
			parentship.children = new GameObjectID[parentship.childrenCount] { 0 };
		}

		DEBUG { spdlog::info ("Creating material -> mesh/es relations."); }

		{	/* It could be replaced with an array of ranges to reference multiple starting points */

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
			{
				auto& shader = canvas.materials[0].program;
				SHADER::Create (shader, svfSimple, sffSimpleRed);
			}
	
			{
				auto& shader = canvas.materials[1].program;
				SHADER::Create (shader, svfColorize, sffColorize);
				SHADER::UNIFORM::Create (shader, mat1USize, mat1UNames, mat1Uniforms );
			}
		}
		
		{
			{
				auto& shader = world.materials[0].program;
				SHADER::Create (shader, svfWorld, sffWorld);
				SHADER::UNIFORM::Create (shader, mat2USize, mat2UNames, mat2Uniforms );
			}
		}
		

		DEBUG { spdlog::info ("Creating render meshes."); }

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

		{ // WORLD

			//{ // STATIC Square MESH render.
			//	auto& verticesSize = MESH::DD::SQUARE::VERTICES_COUNT;
			//	auto& vertices = MESH::DD::SQUARE::VERTICES;
			//	auto& indicesSize = MESH::DD::SQUARE::INDICES_COUNT;
			//	auto& indices = MESH::DD::SQUARE::INDICES;
			//	//
			//	auto& mesh = world.meshes[0].base;
			//	//
			//	MESH::VI::CreateVAO (
			//		mesh.vao, mesh.buffers,
			//		verticesSize, vertices,
			//		indicesSize, indices
			//	);
			//	//
			//	mesh.verticiesCount = indicesSize;
			//	mesh.drawFunc = MESH::VI::Draw;
			//}

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
				componentMesh.id = ENTITY_4;
			}

		}

		DEBUG { spdlog::info ("Creating transfroms."); }

		{ // TRANSFORMS
			glm::mat4 globalspace;
			glm::vec3 translation;
			glm::quat rotation;
			glm::vec3 scale;
			glm::vec4 perspective;
			glm::vec3 skew;

			{ // ROOT
				auto& componentTransfrom = world.transforms[0];
				auto& global = componentTransfrom.global;
				auto& local = componentTransfrom.local;
				//
				local.position	= glm::vec3 (0.0f, -1.0f, 0.0f);
				local.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
				//
				global.position = local.position;
				global.rotation = local.rotation;
				global.scale	= local.scale;
				//
				componentTransfrom.id = ENTITY_3;
			}

			{ // CUBE in ROOT
				auto& componentTransfrom = world.transforms[1];
				auto& global = componentTransfrom.global;
				auto& local = componentTransfrom.local;
				//
				local.position	= glm::vec3 (0.0f, 1.0f, 0.0f);
				local.rotation	= glm::vec3 (15.0f, 25.0f, 35.0f);
				local.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
				//
				globalspace = glm::mat4(1.0f);
				TRANSFORM::ApplyTransform (globalspace, world.transforms[0].local);
				TRANSFORM::ApplyTransform (globalspace, local);
				TRANSFORM::GetUnpacked (globalspace, translation, rotation, scale, perspective, skew);
				//const glm::vec3 result { glm::degrees ( glm::eulerAngles(rotation) ) };
				//
				//global.rotation = glm::eulerAngles (rotation) * 3.14159f / 180.f;
				//const glm::fquat quaternion{glm::radians(eulerAnglesInDegrees)};
				//
				//glm::mat4 transform = glm::eulerAngleYXZ(glm::radians(yawDeg), glm::radians(pitchDeg), glm::radians(rollDeg));
				//glm::mat4 rot = glm::eulerAngleYXZ(global.rotation.y, global.rotation.x, global.rotation.z);
				//
				global.position = translation;
				global.rotation = glm::degrees ( glm::eulerAngles(rotation) );
				global.scale	= scale;
				//
				componentTransfrom.id = ENTITY_4;
				spdlog::info ("rot: {0}, {1}, {2}", global.rotation.x, global.rotation.y, global.rotation.z);
				//spdlog::info ("rot: {0}, {1}, {2}", result.x, result.y, result.z);
				//spdlog::info ("rot: {0}, {1}, {2} {3}", rot.x, rot.y, rot.z, rot.w);
			}



			//{ // ROOT
			//	const auto position = glm::vec3 (0.0f, 0.0f, 0.0f);
			//	const auto rotation = glm::vec3 (0.0f, 0.0f, 0.0f);
			//	const auto scale = glm::vec3 (1.0f, 1.0f, 1.0f);
			//	//
			//	auto& componentTransfrom = world.transforms[0];
			//	auto& transform = componentTransfrom.local;
			//	//
			//	transform = glm::translate (transform, glm::vec3 (position));
			//	transform = glm::scale (transform, scale);
			//	transform = glm::rotate (transform, glm::radians (rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
			//	transform = glm::rotate (transform, glm::radians (rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
			//	transform = glm::rotate (transform, glm::radians (rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
			//	componentTransfrom.id = ENTITY_3;
			//}
			//{ // CUBE in ROOT
			//	const auto position = glm::vec3 (0.0f, 1.0f, 0.0f);
			//	const auto rotation = glm::vec3 (15.0f, 25.0f, 35.0f);
			//	const auto scale = glm::vec3 (1.0f, 1.0f, 1.0f);
			//	//
			//	auto& componentTransfrom = world.transforms[1];
			//	auto& transform = componentTransfrom.local;
			//	//
			//	transform = glm::translate (transform, glm::vec3 (position));
			//	transform = glm::scale (transform, scale);
			//	transform = glm::rotate (transform, glm::radians (rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
			//	transform = glm::rotate (transform, glm::radians (rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
			//	transform = glm::rotate (transform, glm::radians (rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
			//	componentTransfrom.id = ENTITY_4;
			//}

		}

		DEBUG { spdlog::info ("Creating scenegraph."); }

		{ // PARENTSHIPS

			{ // Attach CUBE to ROOT
				auto& componentParentship = world.parentships[0];
				auto& parentship = componentParentship.base;
				//
				parentship.children[0] = ENTITY_4;
				componentParentship.id = ENTITY_3;
			}

		}

		// ? Check if every component has gameObjectId set to value higher then 0.
		// ? Check if in every component that value is unique

		// Connect Scene to Canvas & World structures.
		scene.canvas = &canvas;
		scene.world = &world;

	}

	void Destroy () {

		// Theres no point in setting xyzCount variables to 0.

		DEBUG { spdlog::info ("-> Destroying mesh components."); }

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

		DEBUG { spdlog::info ("-> Destroying transform components."); }

		delete[] canvas.transforms;
		delete[] world.transforms;

		DEBUG { spdlog::info ("-> Destroying parentship components."); }

		for (u64 i = 0; i < canvas.parentshipsCount; ++i) {
			auto& parentship = canvas.parentships[i].base;
			delete[] parentship.children;
		}

		for (u64 i = 0; i < world.parentshipsCount; ++i) {
			auto& parentship = world.parentships[i].base;
			delete[] parentship.children;
		}

		delete[] canvas.parentships;
		delete[] world.parentships;

		DEBUG { spdlog::info ("-> Destroying shader programs."); }

		for (u64 i = 0; i < canvas.materialsCount; ++i) {
			auto& material = canvas.materials[i];
			SHADER::Destroy (material.program);
		}

		for (u64 i = 0; i < world.materialsCount; ++i) {
			auto& material = world.materials[i];
			SHADER::Destroy (material.program);
		}

		DEBUG { spdlog::info ("-> Destroying material collections."); }

		delete[] canvasMaterialMeshes;
		delete[] canvas.materials;

		delete[] worldMaterialMeshes;
		delete[] world.materials;

	}


}