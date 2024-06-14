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
			auto& lTransforms 		= worlds[i].lTransforms;
			auto& gTransforms 		= worlds[i].gTransforms;
			auto& meshTable 		= worlds[i].tables.meshes;
			auto& transformsOffset 	= worlds[i].transformsOffset;

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
		for (u16 i = 0; i < RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT; ++i) {				// Go thouth all gltf difined files.
			auto& json 				= gltfsHandlers[i];

			// WORLD
			auto& parenthoodsCount 	= worlds[i].parenthoodsCount;
			auto& parenthoods 		= worlds[i].parenthoods;
			auto& parenthoodsCT		= parenthoodsChildrenTables[i];
			auto& transformsCount 	= worlds[i].transformsCount;	
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
		}

	}


	void Log () {
		//DEBUG { // TODO: Make it into functions...
		//	auto& world = gltfWorld[0];
		//
		//	spdlog::info ("PARENTHOODS: {0}", world.parenthoodsCount);
		//	for (u16 iParenthood = 0; iParenthood < world.parenthoodsCount; ++iParenthood) {
		//		auto& parenthood = world.parenthoods[iParenthood];
		//
		//		spdlog::info ("p-{0}: id: {1}, cc: {2}", iParenthood, parenthood.id, parenthood.base.childrenCount);
		//		for (u16 iChild = 0; iChild < parenthood.base.childrenCount; ++iChild)
		//			spdlog::info ("id: {0}", parenthood.base.children[iChild]);
		//	}
		//
		//	spdlog::info ("TRANSFORMS: {0}", world.transformsCount);
		//	for (u16 iTransform = 0; iTransform < world.transformsCount; ++iTransform) {
		//		auto& transform = world.lTransforms[iTransform];
		//		auto& base = transform.base;
		//
		//		spdlog::info ("t-{0}: id: {1}", iTransform, transform.id);
		//		spdlog::info ("p: {0}, {1}, {2}", base.position.x, base.position.y, base.position.z);
		//		spdlog::info ("r: {0}, {1}, {2}", base.rotation.x, base.rotation.y, base.rotation.z);
		//		spdlog::info ("s: {0}, {1}, {2}", base.scale.x, base.scale.y, base.scale.z);
		//	}
		//
		//	auto& sWorld = gltfSharedWorld[0];
		//
		//	spdlog::info ("MESHES: {0}", sWorld.meshesCount);
		//	for (u16 iMesh = 0; iMesh < sWorld.meshesCount; ++iMesh) {
		//		auto& mesh = sWorld.meshes[iMesh];
		//
		//		//spdlog::info ("t-{0}: id: {1}", iMesh, transform.id);
		//		//spdlog::info ("p: {0}, {1}, {2}", base.position.x, base.position.y, base.position.z);
		//		//spdlog::info ("r: {0}, {1}, {2}", base.rotation.x, base.rotation.y, base.rotation.z);
		//		//spdlog::info ("s: {0}, {1}, {2}", base.scale.x, base.scale.y, base.scale.z);
		//	}
		//
		//}
	}


	void Destroy () {
		for (u8 igltf = 0; igltf < RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT; ++igltf) { 
			for (u64 iMaterial = 0; iMaterial < sharedWorlds[igltf].materialsCount; ++iMaterial) {
				auto& material = sharedWorlds[igltf].materials[iMaterial];
				SHADER::Destroy (material.program);
			}

			SCENE::WORLD::Destroy (worlds[igltf]);
		}
	}

}
