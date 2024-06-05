#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

#include "util/mmrelation.hpp"

// The Rule Book
// 1. Whats in ROOT-Hierarchy has to have a Transfrom component.
// 2. GameObjectID is an Transform's Component Array id extension
//  Meaning [ 0,1,2 - Transfrom only GM, 3,4,5 - T,MA,ME, 6,7,8 - NO Transform ]
//  This simplifies Transfrom search by a lot!

// 1. Creation Phase
// - Allocate memory for meshTable 
// - Allocate memory for All other Components!
//  ! Parenthood is the worst for the moment as its structure holds a pointer to it's children definition.

// 2. Loading Phase
//  Here exsist 2 different functions as logic for ROOT and normal nodes are different.
//  - Root sets itself (not next Parenthood component) as a parent of it's children.
//  - Root does not add itself as a child to a previous Parenthood component.
//
// - MeshTable
// - Components

namespace RESOURCES::SCENE {

	const char* WORLD = "world";
	const char* CANVAS = "canvas";

	const char* NAME = "name";
	const char* CHILDREN = "children";
	const char* MATERIAL = "material";
	const char* TEXTURE1 = "texture1";
	const char* MESH = "mesh";
	const char* TRANSFORM = "transform";
	const char* POSITION = "position";
	const char* ROTATION = "rotation";
	const char* SCALE = "scale";

	const char* ROTATING = "rotating";

	const char* TRANSFORM_NAMES [3] { POSITION, ROTATION, SCALE };

	const char* D_MATERIAL = "d_material";
	const char* D_MESH = "d_mesh";

	const u8 MATERIAL_INVALID = 255;
	const u8 MESH_INVALID = 255;

}


namespace RESOURCES::SCENE::NODE {

	void NodeCreate (
		/* IN  */ Json& parent,
		/* IN  */ const u8& materialsCount,
		/* IN  */ const u8& meshesCount,
		//
		/* OUT */ u16& mmRelationsLookUpTableSize,
		/* OUT */ u16& mmRelationsLookUpTableCounter,
		/* OUT */ u16*& mmRelationsLookUpTable,
		/* OUT */ u16& relationsLookUpTableOffset,
		/* OUT */ u8& meshTableBytes,
		//
		/* OUT */ u16& parenthoodsCount,
		/* OUT */ u16& childrenSumCount,
		/* OUT */ u16& transformsCount,
		/* OUT */ u16& rotatingsCount
	) {

		u8 materialId = MATERIAL_INVALID;
		u8 meshId = MESH_INVALID;
			
		if ( parent.contains (NAME) ) {
			auto& nodeName = parent[NAME];
		}

		if ( parent.contains (MATERIAL) ) {
			auto& nodeMaterial = parent[MATERIAL];
			materialId = (u8)(nodeMaterial.get<int> ());
		}

		DEBUG if ( parent.contains (D_MATERIAL) ) {
			auto& nodeMaterial = parent[D_MATERIAL];
			materialId = (u8)(nodeMaterial.get<int> ());
		}

		if ( parent.contains (TEXTURE1) ) {
			auto& nodeTexture1 = parent[TEXTURE1];
		}

		if ( parent.contains (MESH) ) {
			auto& nodeMesh = parent[MESH];
			meshId = (u8)(nodeMesh.get<int> ());
		
			if (materialId > materialsCount) ErrorExit ("Selected invalid 'Material': {0}", materialId) // VALIDATION
			if (meshId > meshesCount) ErrorExit ("Selected invalid 'Mesh': {0}", meshId) // VALIDATION

			MMRELATION::CheckAddRelation (
				mmRelationsLookUpTableSize,
				mmRelationsLookUpTableCounter,
				mmRelationsLookUpTable,
				materialId, meshId
			);
		}

		DEBUG if ( parent.contains (D_MESH) ) {
			auto& nodeMesh = parent[D_MESH];
			meshId = nodeMesh.get<int> ();

			if (materialId > materialsCount) ErrorExit ("Selected invalid 'Debug Material': {0}", materialId);

			if (meshId > meshesCount) ErrorExit("Selected invalid 'Debug Mesh': {0}", meshId);

			MMRELATION::CheckAddRelation (
				mmRelationsLookUpTableSize,
				mmRelationsLookUpTableCounter,
				mmRelationsLookUpTable,
				materialId, meshId
			);
		}

		if ( parent.contains (TRANSFORM) ) {
			auto& nodeTransform = parent[TRANSFORM];
			++transformsCount;

			if ((materialId > materialsCount) + (meshId > meshesCount)) {
				MMRELATION::AddEmptyRelation (
					mmRelationsLookUpTableCounter, mmRelationsLookUpTable,
					materialId, meshId
				);
				++relationsLookUpTableOffset;
			}
		}

		if ( parent.contains (ROTATING) ) 
			++rotatingsCount;
		
            
		if ( parent.contains (CHILDREN) ) {
			auto& nodeChildren = parent[CHILDREN];
			auto childrenCount = nodeChildren.size ();

			// Ensure we add a parenthood only when there are defined elements in children node.
			parenthoodsCount += (childrenCount > 0);

			// Get bytes for Parenthood's children allocation.
			childrenSumCount += childrenCount;

			for (u8 iChild = 0; iChild < childrenCount; ++iChild) {
				auto& nodeChild = nodeChildren[iChild];

				NodeCreate (
					nodeChild, materialsCount, meshesCount, 
					mmRelationsLookUpTableSize, mmRelationsLookUpTableCounter, mmRelationsLookUpTable, relationsLookUpTableOffset,
					meshTableBytes, 
					parenthoodsCount, childrenSumCount, transformsCount, rotatingsCount
				);
			}
		}
	}

}


namespace RESOURCES::SCENE {

	void SetMeshTableValue (
		/* OUT */ u8*& meshTable,
		/* IN  */ u16*& relationsLookUpTable,
		/* IN  */ const u16& relationsLookUpTableOffset,
		/* IN  */ const u8& materialId,
		/* IN  */ const u8& meshId
	) {
		// Muszę podliczyć ile skipnąłem materiałów i łącznie meshy!

		// 1 byte offset -> (materials_count)
		// materialID is skippedMaterials ! each is 1 byte -> (meshes_count)
		// relation in releations is skippedMeshes ! each is 2 bytes -> (mesh_id, instances_count)

		// With that we can access right data storage at meshTable.
		//  Now we need to check (instances_count) byte to know whether we're 
		//  creating a new mesh information or just increment instances_count
		// Remember to also update (meshes_count) byte.

		// I should not have duplicates here

		const u16 materialMask = 0b1111'1111'0000'0000;

		auto relations = relationsLookUpTable + relationsLookUpTableOffset;
		auto cashedRelation = MMRELATION::NOT_REPRESENTIVE;
		u16 relation = (materialId << 8) + meshId;
		u16 material = (materialId << 8);

		u16 skippedMeshes = 0; // FIND FIRST OCCURANCE OF SUCH A MMRELATION
		for (u16 iRelation = 0; relations[iRelation] != relation; ++iRelation) {
			// Count only non duplicates!
			if (relations[iRelation] != cashedRelation) {
				cashedRelation = relations[iRelation];
				++skippedMeshes;
			}
		}
			
		// HACK. The elements before index 0 are Relations that we cut off pointer-style
		//  Therefore there is nothing dangerous with 'relations[-1]' check as that memory exsist and it's ours.
		// We do this to get hom many bytes to the left is (meshes_count)
		auto iPreviousRelation = skippedMeshes - 1;
		u16 previousSameMaterialMeshes = 0;
		for (s16 iRelation = iPreviousRelation; iRelation > -1; --iRelation) {
			auto relationMaterial = relations[iRelation] & materialMask;
			if (relationMaterial != material) break;
			++previousSameMaterialMeshes;
		}
			
		auto meshIdByte = 2 + (skippedMeshes * 2) + (materialId * 1);
		auto meshesByte = meshIdByte - 1 - (previousSameMaterialMeshes * 2);
		auto meshInstancesByte = meshIdByte + 1;

		if (meshTable[meshInstancesByte] == 0) {
			++meshTable[meshesByte];
			meshTable[meshIdByte] = meshId;
		}

		++meshTable[meshInstancesByte];
	}

	void Create (
		/* IN  */ Json& json,
		//
		/* IN  */ const u8& materialIds,
		/* IN  */ const u8& mesheIds,
		//
		/* OUT */ u8*& meshTable,
		//
		/* OUT */ u16*& childrenTable,
		/* OUT */ u16*& relationsLookUpTable,
		/* OUT */ u16& relationsLookUpTableOffset,
		//
		/* OUT */ u16& parenthoodsCount,
		/* OUT */ u16& transformsCount,
		/* OUT */ u16& rotatingsCount
	) {
		PROFILER { ZoneScopedN("RESOURCES::SCENE: Create"); }
		DEBUG { spdlog::info ("JSON Scene Initialization"); }

		// Having posible materialIds, and mesheIds
		//  we should here allocate memory for meshTable moving that logic from material to here.

		// We initialize it with 1 because theres 1 byte representing materials count.
		// And theres a byte for each material to represent how many different meshes to render it has.
		u8 meshTableBytes = 1 + materialIds;

		// Create a lookup table !
		// FOR NOW IT"S SIZE is 'materialIds' * 'mesheIds', be aware that this might be unnecessery to much !
		//  however it's memory allocated just for initialization only.

		// u16 -> 1byte material, 1 byte mesh 
		// During node creation after validation we 
		//  - check with exsisting elements whether a such relation exsists or not
		//  - if it doesn't we push_back a new relation for later check and we add up 2 bytes (mesh_id, instances_count) 
			
		relationsLookUpTable = (u16*) malloc (MMRELATION::MAX_NODES * sizeof (u16));
		u16 relationsLookUpTableNonDuplicates = 0;
		u16 relationsLookUpTableCounter = 0;

		u16 childrenSumCount = 0; 						//  We'll allocate it with one call but make it point to different pointers later.

		DEBUG { // DEBUG only validation.
			auto& nodeWorld = json[WORLD];
			if ( json.contains (WORLD) || json[WORLD].size() != 0 ) {

				NODE::NodeCreate (
					nodeWorld, materialIds, mesheIds, 
					relationsLookUpTableNonDuplicates, relationsLookUpTableCounter, relationsLookUpTable, relationsLookUpTableOffset,
					meshTableBytes, 
					parenthoodsCount, childrenSumCount, transformsCount, rotatingsCount
				);

				if (relationsLookUpTableCounter > MMRELATION::MAX_NODES)
					ErrorExit ("Implementation doesn't support more then {0} nodes inside a scene/world", MMRELATION::MAX_NODES);

			} else ErrorExit ("Scene does not contain a valid 'world' key!");

		} else {
			auto& nodeWorld = json[WORLD];

			NODE::NodeCreate (
				nodeWorld, materialIds, mesheIds, 
				relationsLookUpTableNonDuplicates, relationsLookUpTableCounter, relationsLookUpTable, relationsLookUpTableOffset,
				meshTableBytes, 
				parenthoodsCount, childrenSumCount, transformsCount, rotatingsCount
			);

		}

		meshTableBytes += (relationsLookUpTableNonDuplicates) * 2;

		// Allocate memory
		meshTable = (u8*) calloc (meshTableBytes, sizeof (u8));
		childrenTable = (u16*) malloc (childrenSumCount * sizeof (u16));

		MMRELATION::SortRelations (transformsCount, relationsLookUpTable);	
	}
		
	void NodeLoad (
		/* IN  */ Json& parent,
		//
		/* OUT */ u16*& childrenTable,
		/* IN  */ u16*& relationsLookUpTable,
		/* IN  */ const u16& relationsLookUpTableOffset,
		// COMPONENTS
		/* OUT */ u8*& meshTable,
		/* OUT */ u8& childCounter, 
		/* OUT */ PARENTHOOD::Parenthood* parenthoods, 
		/* OUT */ u16& transformsCounter, 
		/* OUT */ TRANSFORM::LTransform* transforms,
		/* OUT */ u16& rotatingsCounter, 
		/* OUT */ ROTATING::Rotating*& rotatings
	) {
		u8 materialId = MATERIAL_INVALID;
		u8 meshId = MESH_INVALID;

		// This is Transform index and GameObject id
		u16 validKeyPos = MMRELATION::NOT_REPRESENTIVE;
			
		if ( parent.contains (NAME) ) {
			auto& nodeName = parent[NAME];
		}

		if ( parent.contains (MATERIAL) ) {
			auto& nodeMaterial = parent[MATERIAL];
			materialId = nodeMaterial.get<int> ();
		}

		DEBUG if ( parent.contains (D_MATERIAL) ) {
			auto& nodeMaterial = parent[D_MATERIAL];
			materialId = nodeMaterial.get<int> ();
		}

		if ( parent.contains (TEXTURE1) ) {
			auto& nodeTexture1 = parent[TEXTURE1];
		}

		if ( parent.contains (MESH) ) {
			auto& nodeMesh = parent[MESH];
			meshId = nodeMesh.get<int> ();

			if (materialId != MATERIAL_INVALID) {
				SetMeshTableValue (meshTable, relationsLookUpTable, relationsLookUpTableOffset, materialId, meshId);
			}
		}

		DEBUG if ( parent.contains (D_MESH) ) {
			auto& nodeMesh = parent[D_MESH];
			meshId = nodeMesh.get<int> ();

			if (materialId != MATERIAL_INVALID) {
				SetMeshTableValue (meshTable, relationsLookUpTable, relationsLookUpTableOffset, materialId, meshId);
			}
		}

		if ( parent.contains (TRANSFORM) ) {
			auto& nodeTransform = parent[TRANSFORM];

			TRANSFORM::LTransform tempTransform { 0 };  // Initialize for simplicity for now.

			{ // READ 
				r32* transform = (r32*) (void*) &(tempTransform.base);
				for (u8 iNode = 0; iNode < 3; ++iNode) {
					auto& node = nodeTransform[TRANSFORM_NAMES[iNode]];
					for (u8 iValue = 0; iValue < 3; ++iValue) {
						auto& value = node[iValue];
						transform[iNode * 3 + iValue] = value.get<float> ();
					}
				}
			}
			
			u16 relation = (materialId << 8) + meshId;

			// FIND FIRST OCCURANCE OF SUCH A MMRELATION
			u16 iTransform = 0; for (; relationsLookUpTable[iTransform] != relation; ++iTransform);

			// IF it's already set look for next spot. // HACK!!! we assume scale is always non 0.
			validKeyPos = iTransform; for (; transforms[validKeyPos].base.scale.x != 0; ++validKeyPos);

			// FINALLY SET
			transforms[validKeyPos].base = tempTransform.base;
			transforms[validKeyPos].id = transformsCounter;
			++transformsCounter;

			// Now I need to set up Parenthoods correctly
			//  Which is When a node has children we assign
			//  to an unused parenthood 
			//  parent value -> transformsCounter
			//  child value -> child's transfromsCounter
			// Also Systems->GetFast have to be changed to GetSlow!
			//  No wait. if GameObjectID is connected to transfroms then theres an easier / better way to write that.

			auto& currParenthood = parenthoods[0];
			currParenthood.base.children[childCounter] = validKeyPos;
			++childCounter;
		}

		if ( parent.contains (ROTATING) ) {
			auto& nodeRotating = parent[ROTATING];
			auto& rotating = rotatings[rotatingsCounter].base;

			// MISSING! CHECK IF TRANSFROM COMPONENT IS PRESENT !

			for (u8 iValue = 0; iValue < 3; ++iValue) {
				auto& value = nodeRotating[iValue];
				rotating[iValue] = value.get<float> ();
			}

			rotatings[rotatingsCounter].id = validKeyPos;
			++rotatingsCounter;
		}
            
		if ( parent.contains (CHILDREN) ) {
			auto& nodeChildren = parent[CHILDREN];
			auto childrenCount = nodeChildren.size ();

			auto currParenthood = parenthoods + 1;
			currParenthood[0].id = validKeyPos;
			currParenthood[0].base.childrenCount = childrenCount;
			currParenthood[0].base.children = childrenTable;
			
			childrenTable += childrenCount; 							// 'equals' So we don't overlap parenthood trees with each child children.
			u8 childchildrenCounter = 0; 								// Create a new counter (remember recursive!)

			for (u8 iChild = 0; iChild < childrenCount; ++iChild) {
				auto& nodeChild = nodeChildren[iChild];
				NodeLoad (
					nodeChild, childrenTable, 
					relationsLookUpTable, relationsLookUpTableOffset,
					meshTable,
					childchildrenCounter, currParenthood, 				// So we would refer to the next one.
					transformsCounter, transforms,
					rotatingsCounter, rotatings
				);
			}
		}
	}


	void NodeRootLoad (
		/* IN  */ Json& parent,
		//
		/* OUT */ u16* childrenTable,					/* CPY */
		/* IN  */ u16*& relationsLookUpTable,
		/* IN  */ const u16& relationsLookUpTableOffset,
		// COMPONENTS
		/* OUT */ u8*& meshTable,
		/* OUT */ u8& childCounter, 
		/* OUT */ PARENTHOOD::Parenthood* parenthoods, 
		/* OUT */ u16& transformsCounter, 
		/* OUT */ TRANSFORM::LTransform* transforms,
		/* OUT */ u16& rotatingsCounter, 
		/* OUT */ ROTATING::Rotating*& rotatings
	) {
		u8 materialId = MATERIAL_INVALID;
		u8 meshId = MESH_INVALID;

		// Transform index and GameObject id
		u16 validKeyPos = MMRELATION::NOT_REPRESENTIVE;
			
		if ( parent.contains (NAME) ) {
			auto& nodeName = parent[NAME];
		}

		if ( parent.contains (MATERIAL) ) {
			auto& nodeMaterial = parent[MATERIAL];
			materialId = nodeMaterial.get<int> ();
		}

		if ( parent.contains (D_MATERIAL) ) {
			auto& nodeMaterial = parent[D_MATERIAL];
			materialId = nodeMaterial.get<int> ();
		}

		if ( parent.contains (TEXTURE1) ) {
			auto& nodeTexture1 = parent[TEXTURE1];
		}

		DEBUG if ( parent.contains (MESH) ) {
			auto& nodeMesh = parent[MESH];
			meshId = nodeMesh.get<int> ();
		}

		DEBUG if ( parent.contains (D_MESH) ) {
			auto& nodeMesh = parent[D_MESH];
			meshId = nodeMesh.get<int> ();
		}

		if ( parent.contains (TRANSFORM) ) {
			auto& nodeTransform = parent[TRANSFORM];

			TRANSFORM::LTransform tempTransform { 0 };  // Initialize for simplicity for now.

			{ // READ 
				r32* transform = (r32*) (void*) &(tempTransform.base);
				for (u8 iNode = 0; iNode < 3; ++iNode) {
					auto& node = nodeTransform[TRANSFORM_NAMES[iNode]];
					for (u8 iValue = 0; iValue < 3; ++iValue) {
						auto& value = node[iValue];
						transform[iNode * 3 + iValue] = value.get<float> ();
					}
				}
			}
				
			u16 relation = (materialId << 8) + meshId;

			// FIND FIRST OCCURANCE OF SUCH A MMRELATION
			u16 iTransform = 0; for (; relationsLookUpTable[iTransform] != relation; ++iTransform);
			// IF it's already set look for next spot. // HACK!!! we assume scale is always non 0.
			validKeyPos = iTransform; for (; transforms[validKeyPos].base.scale.x != 0; ++validKeyPos);
			
			// FINALLY SET
			transforms[validKeyPos].base = tempTransform.base;
			transforms[validKeyPos].id = transformsCounter;
			++transformsCounter;
		}

		if ( parent.contains (ROTATING) ) {
			auto& nodeRotating = parent[ROTATING];
			auto& rotating = rotatings[rotatingsCounter].base;

			// MISSING! CHECK IF TRANSFROM COMPONENT IS PRESENT !

			for (u8 iValue = 0; iValue < 3; ++iValue) {
				auto& value = nodeRotating[iValue];
				rotating[iValue] = value.get<float> ();
			}

			rotatings[rotatingsCounter].id = validKeyPos;
			++rotatingsCounter;
		}
            
		if ( parent.contains (CHILDREN) ) {
			auto& nodeChildren = parent[CHILDREN];
			auto childrenCount = nodeChildren.size ();

			// ROOT
			auto& root = parenthoods[0];
			root.id = 0;
			root.base.childrenCount = childrenCount;
			root.base.children = childrenTable;

			childrenTable += childrenCount; 						// 'equals' So we don't overlap parenthood trees with each child children.
			u8 childchildrenCounter = 0; 							// Create a new counter (remember recursive!)

			for (u8 iChild = 0; iChild < childrenCount; ++iChild) {
				auto& nodeChild = nodeChildren[iChild];
				NodeLoad (
					nodeChild, childrenTable, 
					relationsLookUpTable, relationsLookUpTableOffset,
					meshTable,
					childchildrenCounter, parenthoods, 				// So we would refer to the next one.
					transformsCounter, transforms,
					rotatingsCounter, rotatings
				);
			}
		} 
	}


	void Load (
		/* IN  */ Json& json,
		// MATERIAL-MESH
		/* IN  */ const u8& materialIds, 
		/* IN  */ const u8& meshesIds, 
		/* OUT */ u8*& meshTable,
		//
		/* OUT */ u16*& childrenTable,
		/* IN  */ u16*& relationsLookUpTable,
		/* IN  */ const u16& relationsLookUpTableOffset,
		// COMPONENTS
		/* IN  */ const u16& parenthoodsCount, 
		/* OUT */ PARENTHOOD::Parenthood*& parenthoods, 
		/* IN  */ const u16& transformsCount, 
		/* OUT */ TRANSFORM::LTransform*& transforms,
		/* IN  */ const u16& rotatingsCount, 
		/* OUT */ ROTATING::Rotating*& rotatings
	) {
		meshTable[0] = materialIds;									// Setup material count inside the table.

		u8 rootChildrenCounter = 0;
		u16 transformsCounter = 0;
		u16 rotatingsCounter = 0;

		auto& nodeWorld = json[WORLD];

		NodeRootLoad ( 
			nodeWorld, childrenTable, 
			relationsLookUpTable, relationsLookUpTableOffset,
			meshTable,
			rootChildrenCounter, parenthoods,
			transformsCounter, transforms,
			rotatingsCounter, rotatings
		);

		free (relationsLookUpTable);								// Finally free relations "LUT"
	}

}