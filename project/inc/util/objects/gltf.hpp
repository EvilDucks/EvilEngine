#pragma once
#include "resources/gltf.hpp"
#include "scene.hpp"

//namespace GLOBAL {
//
//	SCENE::SHARED::World    gltfSharedWorld[RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT];
//	SCENE::World            gltfWorld[RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT];
//
//}

// TODO
// 1. transformsOffset -> Calculate transfroms without meshes.
// 2. childrenTable -> Calculate how many childen there are and allocate an array. For Parenthood components to use.

namespace OBJECTS::GLTF {

	SCENE::SHARED::World    gltfSharedWorld[RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT];
	SCENE::World            gltfWorld[RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT];

	void Create () {

		auto& handlersCount = RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT;
		
		u16*	parenthoodsChildrenTable[handlersCount];                        		// We'll allocate it with one call but point to different pointers later.
		u16		childrenCount[handlersCount];                                   		// For each allocation we need to store it's size. For now at least.
		
		u8*		duplicateObjects[handlersCount] 					{ nullptr };		// Will inform us on how many same nodes are being refered in scene tree.
		u8		nodeMeshTable[handlersCount][MESH::MAX_MESHES / 2]	{}; 				// Will inform as about hidden nodes eg. Primitive->Mesh conversion.
		
		RESOURCES::Json 		gltfsHandlers[handlersCount]		{ 0 };				// A nlohmann/json data handler.
		SCENE::SceneLoadContext gltfLoad[handlersCount]         	{ 0 };				// Load structure. aka. relationsLookUpTable -> material+mesh sort sys.
		
		
		for (u8 iHandler = 0; iHandler < handlersCount; ++iHandler) {
			auto& mmrlut = gltfLoad[iHandler].relationsLookUpTable;
			mmrlut = (u16*) malloc (RESOURCES::MMRELATION::MAX_NODES * sizeof (u16));	// MMRLUT ALLOCATION !
		}
		
		for (u16 i = 0; i < RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT; ++i) {			// GLTF files enumeration.
			auto& filepath 			= RESOURCES::MANAGER::GLTFS::FILEPATHS[i];						
			auto& json 				= gltfsHandlers[i];
		
			// WORLD
			auto& parenthoodsCount 	= gltfWorld[i].parenthoodsCount;
			auto& parenthoods 		= gltfWorld[i].parenthoods;
			auto& transformsCount 	= gltfWorld[i].transformsCount;	
			auto& lTransforms 		= gltfWorld[i].lTransforms;
			auto& gTransforms 		= gltfWorld[i].gTransforms;
			auto& meshTable 		= gltfWorld[i].tables.meshes;
			auto& transformsOffset 	= gltfWorld[i].transformsOffset;
		
			// SHARED 
			auto& materialsCount 	= gltfSharedWorld[i].materialsCount;
			auto& materials 		= gltfSharedWorld[i].materials;
			auto& meshesCount 		= gltfSharedWorld[i].meshesCount;
			auto& meshes 			= gltfSharedWorld[i].meshes;
		
			DEBUG_ENGINE spdlog::info ("Creating gltf: {0}.", filepath);

			RESOURCES::Parse (json, filepath);											// Parse the file into json format.

			RESOURCES::GLTF::Create (													// Get elements information from json into engine format.
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
				transformsOffset,
				//
				duplicateObjects[i],
				nodeMeshTable[i]
			);	
		
			RESOURCES::GLTF::Allocate (													// Allocate memory for all the pre-read commponents.
				childrenCount[i], parenthoodsChildrenTable[i], 
				parenthoodsCount,  parenthoods,
				transformsCount, lTransforms, gTransforms,
				materialsCount, materials,
				meshesCount, meshes
			);
			
		}

	}

	/*
	//for (u16 i = 0; i < RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT; ++i) {				// Go thouth all gltf difined files.
		//	auto& json 				= gltfsHandlers[i];
		//
		//	// WORLD
		//	auto& parenthoodsCount 	= gltfWorld[i].parenthoodsCount;
		//	auto& parenthoods 		= gltfWorld[i].parenthoods;
		//	auto& transformsCount 	= gltfWorld[i].transformsCount;	
		//	auto& lTransforms 		= gltfWorld[i].lTransforms;
		//	auto& gTransforms 		= gltfWorld[i].gTransforms;
		//	auto& meshTable 		= gltfWorld[i].tables.meshes;
		//
		//	// SHARED 
		//	auto& materialsCount 	= gltfSharedWorld[i].materialsCount;
		//	auto& materials 		= gltfSharedWorld[i].materials;
		//	auto& meshesCount 		= gltfSharedWorld[i].meshesCount;
		//	auto& meshes 			= gltfSharedWorld[i].meshes;
		//
		//	DEBUG spdlog::info ("Loading gltf: {0}.", i);
		//	RESOURCES::GLTF::Load (															// Parse json in engine format. 
		//		json, gltfLoad[i],
		//		//
		//		parenthoodsCount,
		//		parenthoodsChildrenTable[i],
		//		parenthoods,
		//		//
		//		transformsCount,
		//		lTransforms,
		//		gTransforms,
		//		//
		//		materialsCount,
		//		materials,
		//		//
		//		meshesCount,
		//		meshes,
		//		//
		//		meshTable,
		//		//
		//		duplicateObjects[i],
		//		nodeMeshTable[i]
		//	);													
		//}

		// spdlog::info ("HERE!");


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

		DEBUG_ENGINE spdlog::info ("Combining and Sorting the scenes.");

		//
	*/


	void Destroy () {
		auto& handlersCount = RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT;

		for (u8 igltf = 0; igltf < handlersCount; ++igltf) { 
			auto& sharedWorld = gltfSharedWorld[igltf];
			auto& world = gltfWorld[igltf];

			for (u64 iMaterial = 0; iMaterial < sharedWorld.materialsCount; ++iMaterial) {
				auto& material = sharedWorld.materials[iMaterial];
				SHADER::Destroy (material.program);
			}

			SCENE::WORLD::Destroy (world);
		}
	}

}
