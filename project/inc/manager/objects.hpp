#pragma once

#include "resources/manager.hpp"
#include "scene.hpp"

// TODO
// 1. transformsOffset -> Calculate transfroms without meshes.
// 2. childrenTable -> Calculate how many childen there are and allocate an array. For Parenthood components to use.

namespace MANAGER::OBJECTS::GLTF {

	// GLTF world & shared world containers
	//  remember we treat them as prefabs.
	//

	SCENE::SHARED::World sharedWorlds[RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT];		//
	SCENE::World worlds[RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT];						//

	RESOURCES::Json gltfsHandlers[RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT]	{ 0 };	// Create an a array of nlohmann/json data handlers.
	RESOURCES::GLTF::LoadHelper gltfLoad[RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT] { 0 };					// Create a load structure. aka. relationsLookUpTable.

	//  We'll creating space to hold all gltf-scene children nodeIds inside later we manipulate that pointer
	//   to properly assign them children to theirs parents.
	u16* parenthoodsChildrenTables[RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT];			// Parenthoods children pointer

}






namespace MANAGER::OBJECTS::GLTF::MATERIALS {

	void CreateSimple (
		const u8& materialsCount,
		MATERIAL::Material*& materials,
		u8*& tableShaders, 
		u8*& tableUniforms
	) {
		//const char tableShadersData[] = "DebugBlue\0" "SpaceOnly.vert\0" "SimpleBlue.frag\0" "\2"->this value is in octagonal "view\0" "projection";
		const char tableShadersData[] = "DebugBlue\0" "gltf.vert\0" "SimpleColor.frag\0" "\13" "view\0" "projection\0" "color\0"
                                        "uLight.position\0" "uLight.attenuation.constant\0" "uLight.attenuation.linear\0"
                                        "uLight.attenuation.quadratic\0" "uLight.base.ambient\0" "uLight.base.ambientIntensity\0"
                                        "uLight.base.diffuse\0" "uLight.base.diffuseIntensity";
		const u8 UNIFORMS_COUNT = 11;

		const u64 tableShadersByteCount = 1 + (sizeof (tableShadersData) * materialsCount);
		const u64 tableUniformsByteCount = 1  + ( 1  + SHADER::UNIFORM::UNIFORM_BYTES * UNIFORMS_COUNT) * materialsCount;

		tableShaders = (u8*) malloc (tableShadersByteCount * sizeof (u8));
		tableUniforms = (u8*) malloc (tableUniformsByteCount * sizeof (u8));

		{ // SET tableShaders
			auto& shadersCount = tableShaders[0];
			shadersCount = materialsCount;

			u16 counter = 1;
			for (u16 iShader = 0; iShader < shadersCount; ++iShader) {
				for (u16 iCharacter = 0; iCharacter < sizeof (tableShadersData); ++iCharacter) {
					tableShaders[counter] = tableShadersData[iCharacter];
					++counter;
				}
			}
		}

		{ // SET tableUniforms
			u16 tableUniformsBytesRead = 0;
			auto& shadersCount = tableUniforms[0];

			auto& projection = SHADER::UNIFORM::uniforms[0];
			auto& view = SHADER::UNIFORM::uniforms[1];
			auto& color = SHADER::UNIFORM::uniforms[5];
			auto& uLightPos = SHADER::UNIFORM::lightPosition;
            auto& uLightAttenuationConstant = SHADER::UNIFORM::lightConstant;
            auto& uLightAttenuationLinear = SHADER::UNIFORM::lightLinear;
            auto& uLightAttenuationQuadratic = SHADER::UNIFORM::lightQuadratic;
            auto& uLightAmbient = SHADER::UNIFORM::lightAmbient;
            auto& uLightAmbientIntensity = SHADER::UNIFORM::lightAmbientIntensity;
            auto& uLightDiffuse = SHADER::UNIFORM::lightDiffuse;
            auto& uLightDiffuseIntensity = SHADER::UNIFORM::lightDiffuseIntensity;

			shadersCount = materialsCount;

			for (u16 iShader = 0; iShader < shadersCount; ++iShader) {
				const auto&& uniformsRange = SIZED_BUFFOR::GetCount (tableUniforms, iShader, tableUniformsBytesRead);

				auto& uniformsCount = *(uniformsRange);
				auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);

				uniformsCount = UNIFORMS_COUNT;
				uniforms[0] = view;
				uniforms[1] = projection;
				uniforms[2] = color;
				uniforms[3] = uLightPos;
				uniforms[4] = uLightAttenuationConstant;
				uniforms[5] = uLightAttenuationLinear;
				uniforms[6] = uLightAttenuationQuadratic;
				uniforms[7] = uLightAmbient;
				uniforms[8] = uLightAmbientIntensity;
				uniforms[9] = uLightDiffuse;
				uniforms[10] = uLightDiffuseIntensity;

				tableUniformsBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
			}
		}
	}


	void CreateGooch (
		const u8& materialsCount,
		MATERIAL::Material*& materials,
		u8*& tableShaders, 
		u8*& tableUniforms
	) {
		const char tableShadersData[] = 
			"Gooch\0" "Gooch.vert\0" "GoochFace.frag\0" 
			"\x8" "view\0" "projection\0" "sampler1\0" "lightAmbient\0" "laIntensity\0" "lightDiffuse\0" "ldPosition\0" "ldIntensity";
		
		// \x8  -> hex 8
		// \10  -> oct 8

		const u8 UNIFORMS_COUNT = 8;

		const u64 tableShadersByteCount = 1 + (sizeof (tableShadersData) * materialsCount);
		const u64 tableUniformsByteCount = 1  + ( 1  + SHADER::UNIFORM::UNIFORM_BYTES * UNIFORMS_COUNT) * materialsCount;

		tableShaders = (u8*) malloc (tableShadersByteCount * sizeof (u8));
		tableUniforms = (u8*) malloc (tableUniformsByteCount * sizeof (u8));

		{ // SET tableShaders
			auto& shadersCount = tableShaders[0];
			shadersCount = materialsCount;

			u16 counter = 1;
			for (u16 iShader = 0; iShader < shadersCount; ++iShader) {
				for (u16 iCharacter = 0; iCharacter < sizeof (tableShadersData); ++iCharacter) {
					tableShaders[counter] = tableShadersData[iCharacter];
					++counter;
				}
			}
		}

		{ // SET tableUniforms
			u16 tableUniformsBytesRead = 0;
			auto& shadersCount = tableUniforms[0];

			auto& projection	= SHADER::UNIFORM::uniforms[0];
			auto& view			= SHADER::UNIFORM::uniforms[1];
			auto& sampler1		= SHADER::UNIFORM::uniforms[3];
			auto& lightAmbient	= SHADER::UNIFORM::uniforms[13];
			auto& laIntensity	= SHADER::UNIFORM::uniforms[14];
			auto& lightDiffuse	= SHADER::UNIFORM::uniforms[15];
			auto& ldPosition	= SHADER::UNIFORM::uniforms[8];
			auto& ldIntensity	= SHADER::UNIFORM::uniforms[16];

			shadersCount = materialsCount;

			for (u16 iShader = 0; iShader < shadersCount; ++iShader) {
				const auto&& uniformsRange = SIZED_BUFFOR::GetCount (tableUniforms, iShader, tableUniformsBytesRead);

				auto& uniformsCount = *(uniformsRange);
				auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);

				uniformsCount = UNIFORMS_COUNT;
				uniforms[0] = view;
				uniforms[1] = projection;
				uniforms[2] = sampler1;
				uniforms[3] = lightAmbient;
				uniforms[4] = laIntensity;
				uniforms[5] = lightDiffuse;
				uniforms[6] = ldPosition;
				uniforms[7] = ldIntensity;

				tableUniformsBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
			}
		}
	}

}




namespace MANAGER::OBJECTS::GLTF {

	void Create () {
		auto& handlersCount = RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT;

		u16 childrenCount[handlersCount];

		// Pre alloc moved to surface.
		for (u8 i = 0; i < handlersCount; ++i) {
			gltfLoad[i].mmrlut = (u16*) malloc (RESOURCES::MMRELATION::MAX_NODES * sizeof (u16));
			gltfLoad[i].nodeMeshTable = (u8*) malloc (MESH::MAX_MESHES / 2 * sizeof (u8));
		}

		//u8* duplicateObjects[handlersCount] 					{ nullptr };	
		//u8 nodeMeshTable[handlersCount][MESH::MAX_MESHES / 2]	{}; 			

		for (u16 i = 0; i < RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT; ++i) {	// Go thouth all gltf difined files.
			auto& filepath 			= RESOURCES::MANAGER::GLTFS::FILEPATHS[i];						
			auto& json 				= gltfsHandlers[i];

			// WORLD
			auto& parenthoodsCount 	= worlds[i].parenthoodsCount;
			auto& parenthoods 		= worlds[i].parenthoods;
			auto& parenthoodsCT		= parenthoodsChildrenTables[i];
			auto& transformsCount 	= worlds[i].transformsCount;	
			auto& transformsOffset 	= worlds[i].transformsOffset;
			auto& lTransforms 		= worlds[i].lTransforms;
			auto& gTransforms 		= worlds[i].gTransforms;
			auto& meshTable 		= worlds[i].tables.meshes;

			// SHARED 
			auto& materialsCount 	= sharedWorlds[i].materialsCount;
			auto& materials 		= sharedWorlds[i].materials;
			auto& meshesCount 		= sharedWorlds[i].meshesCount;
			auto& meshes 			= sharedWorlds[i].meshes;

			DEBUG_GLTF spdlog::info ("Creating gltf: {0}.", filepath);
			RESOURCES::Parse (json, filepath);												// Parse file into json format.
			RESOURCES::GLTF::Create (														// Parse json in engine format. (Allocation and helper structs inforamtion only)
				json, gltfLoad[i],
				//
				parenthoodsCount,
				childrenCount[i],
				transformsCount,
				//
				materialsCount,
				meshesCount,
				//
				meshTable,
				transformsOffset
			);	

			RESOURCES::GLTF::Allocate (
				childrenCount[i], parenthoodsCT, 
				parenthoodsCount,  parenthoods,
				transformsCount, lTransforms, gTransforms,
				materialsCount, materials,
				meshesCount, meshes
			);
			
		}
	}

	void Load () {
		// u16 -> u32 conversion
		RESOURCES::GLTF::FILE::indicesEx = (u32*) calloc (RESOURCES::GLTF::FILE::indicesExCount, sizeof(u32));

		for (u16 i = 0; i < RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT; ++i) {				// Go thouth all gltf difined files.
			auto& json 				= gltfsHandlers[i];

			// WORLD
			auto& parenthoodsCount 	= worlds[i].parenthoodsCount;
			auto& parenthoods 		= worlds[i].parenthoods;
			auto& parenthoodsCT		= parenthoodsChildrenTables[i];
			auto& transformsCount 	= worlds[i].transformsCount;	
			auto& transformsOffset 	= worlds[i].transformsOffset;
			auto& lTransforms 		= worlds[i].lTransforms;
			auto& gTransforms 		= worlds[i].gTransforms;
			auto& meshTable 		= worlds[i].tables.meshes;

			// SHARED 
			auto& materialsCount 	= sharedWorlds[i].materialsCount;
			auto& materials 		= sharedWorlds[i].materials;
			auto& meshesCount 		= sharedWorlds[i].meshesCount;
			auto& meshes 			= sharedWorlds[i].meshes;

			DEBUG_GLTF spdlog::info ("Loading gltf: {0}.", i);
			RESOURCES::GLTF::Load (															// Parse json in engine format. 
				json, gltfLoad[i],
				//
				parenthoodsCount,
				parenthoodsCT,
				parenthoods,
				//
				transformsCount,
				transformsOffset,
				lTransforms,
				gTransforms,
				//
				materialsCount,
				materials,
				//
				meshesCount,
				meshes,
				//
				meshTable
			);		

			MATERIALS::CreateSimple (
				sharedWorlds[i].materialsCount, 
				sharedWorlds[i].materials, 
				sharedWorlds[i].loadTables.shaders, 
				sharedWorlds[i].tables.uniforms
			);

			//MATERIALS::CreateGooch (
			//	sharedWorlds[i].materialsCount, 
			//	sharedWorlds[i].materials, 
			//	sharedWorlds[i].loadTables.shaders, 
			//	sharedWorlds[i].tables.uniforms
			//);
														
		}

		delete[] RESOURCES::GLTF::FILE::indicesEx;
	}

	void Log (
		const SCENE::World& world, 
		const SCENE::SHARED::World& sWorld
	) {
		DEBUG {

			spdlog::info ("PARENTHOODS: {0}", world.parenthoodsCount);
			for (u16 iParenthood = 0; iParenthood < world.parenthoodsCount; ++iParenthood) {
				auto& parenthood = world.parenthoods[iParenthood];
		
				spdlog::info ("p-{0}: id: {1}, cc: {2}", iParenthood, parenthood.id, parenthood.base.childrenCount);
				for (u16 iChild = 0; iChild < parenthood.base.childrenCount; ++iChild)
					spdlog::info (" id: {0}", parenthood.base.children[iChild]);
			}
		
			spdlog::info ("TRANSFORMS: {0}", world.transformsCount);
			for (u16 iTransform = 0; iTransform < world.transformsCount; ++iTransform) {
				auto& transform = world.lTransforms[iTransform];
				auto& base = transform.base;
		
				spdlog::info ("t-{0}: id: {1}", iTransform, transform.id);
				spdlog::info (" p: {0}, {1}, {2}", base.position.x, base.position.y, base.position.z);
				spdlog::info (" r: {0}, {1}, {2}", base.rotation.x, base.rotation.y, base.rotation.z);
				spdlog::info (" s: {0}, {1}, {2}", base.scale.x, base.scale.y, base.scale.z);
			}
		
			spdlog::info ("MESHES: {0}", sWorld.meshesCount);
			for (u16 iMesh = 0; iMesh < sWorld.meshesCount; ++iMesh) {
				auto& mesh = sWorld.meshes[iMesh].base;

				spdlog::info ("m-{0}:", iMesh);
				spdlog::info (" vc: {0}", mesh.verticiesCount);
				spdlog::info (" bc: {0}", mesh.buffersCount);
				spdlog::info (" df: {0}", (u64)mesh.drawFunc);
				spdlog::info (" bn: {0}, {1}, {2}", mesh.boundsMin.x, mesh.boundsMin.y, mesh.boundsMin.z);
				spdlog::info (" bm: {0}, {1}, {2}", mesh.boundsMax.x, mesh.boundsMax.y, mesh.boundsMax.z);
				spdlog::info (" br: {0}", mesh.boundsRadius);
			}

			spdlog::info ("MATERIALS: {0}", sWorld.materialsCount);
			for (u16 iMaterial = 0; iMaterial < sWorld.materialsCount; ++iMaterial) {
				auto& material = sWorld.materials[iMaterial];
				spdlog::info ("m-{0}:", iMaterial);
				spdlog::info (" p: {0}", material.program.id);
				spdlog::info (" t: {0}", material.texture);
			}

			auto&& shadersTable = sWorld.loadTables.shaders;
			RESOURCES::SHADERS::LogShaders (shadersTable);

			auto&& uniformsTable = sWorld.tables.uniforms;
			RESOURCES::SHADERS::LogUniforms (uniformsTable);
		
		}
	}


	void Set () {
		for (u16 i = 0; i < RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT; ++i) {	
			DEBUG spdlog::info ("--------------------------------------------------------");
			TRANSFORM::Precalculate ( // Prep for GPU use.
				worlds[i].parenthoodsCount, worlds[i].parenthoods,
				worlds[i].transformsCount, worlds[i].lTransforms, worlds[i].gTransforms
			);

			//Log (worlds[i], sharedWorlds[i]);

			RESOURCES::SHADERS::Load (  // Load into GPU.
				RESOURCES::MANAGER::SHADERS_WORLD_SIZE, RESOURCES::MANAGER::SHADERS_WORLD, 
				sharedWorlds[i].loadTables.shaders, sharedWorlds[i].tables.uniforms, sharedWorlds[i].materials 
			);
		}
	}


	void Destroy () {
		for (u8 igltf = 0; igltf < RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT; ++igltf) { 
			delete[] sharedWorlds[igltf].tables.uniforms;
			delete[] sharedWorlds[igltf].loadTables.shaders;
			delete[] sharedWorlds[igltf].materials;

			for (u64 iMaterial = 0; iMaterial < sharedWorlds[igltf].materialsCount; ++iMaterial) {
				auto& material = sharedWorlds[igltf].materials[iMaterial];
				SHADER::Destroy (material.program);
			}

			SCENE::WORLD::Destroy (worlds[igltf]);

			//RESOURCES::MATERIALS::DestoryMaterials (
			//	sharedScreen.tables.uniforms, screen.tables.meshes, sharedScreen.materials,
			//	sharedCanvas.tables.uniforms, canvas.tables.meshes, sharedCanvas.materials,
			//	sharedWorld.tables.uniforms, sharedWorld.materials
			//);

			//RESOURCES::MATERIALS::DestroyLoadShaders (
			//	sharedScreen.loadTables.shaders, sharedCanvas.loadTables.shaders, sharedWorld.loadTables.shaders
			//);
		}
	}

}
