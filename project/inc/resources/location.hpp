#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

// The Rule Book
// 1. Whats in ROOT-Hierarchy has to have a Transfrom component.
// 2. GameObjectID is an Transform's Component Array id extension
//  Meaning [ 0,1,2 - Transfrom only GM, 3,4,5 - T,MA,ME, 6,7,8 - NO Transform ]
//  This simplifies Transfrom search by a lot!

// What are relations?
//  Relations is an a array that holds information about read object material+mesh
//   if it was missing a material or a mesh then the relation is set to an invalid, but it
//   is still stored as it is crucial information for later. We create relations during creation phase
//   after it we sort it (invalid ones go to the top, rest is sorted ascending).
//   During loading phase we then find first not already claimed match of current node's material+mesh
//   The index we find is at which index our loaded transform has to be placed inside Trasforms Array.
//   As we finish the loading we then can unallocate all the created relations.
//
//  (We put invalid-keys at the beginning to match them with GameObjects that have Transform components,
//   but don't have Mesh and Material components. This becomes usefull during render as we can then simply offset
//   by the amount of Transform-Only to properly render all Shapes at right positions.)
// 
//  HACK. We use a dirty hack during matching. We assume that scale is not equal 0. 
//   We first use calloc instead of malloc to make a not claimed key.

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


// Things i still need to do / fix.
//  Get each parenthood children count.
//   And allocate that memory.
//  Set meshTable here.
//  Make Release build work. (Due to Debug Meshes and Materials it stopped)


namespace RESOURCES::SCENE {

	const char* NAME = "name";
	const char* CHILDREN = "children";
	const char* MATERIAL = "material";
	const char* TEXTURE1 = "texture1";
	const char* MESH = "mesh";
	const char* TRANSFORM = "transform";
	const char* POSITION = "position";
	const char* ROTATION = "rotation";
	const char* SCALE = "scale";

	const char* TRANSFORM_NAMES [3] { POSITION, ROTATION, SCALE };

	const char* D_MATERIAL = "d_material";
	const char* D_MESH = "d_mesh";

	const u8 MATERIAL_INVALID = 255;
	const u8 MESH_INVALID = 255;

}


namespace RESOURCES::SCENE::RELATION {

	// Objects that don't have a Mesh or Material or both
	//  are marked with the following value as invalid.
	const u16 NOT_REPRESENTIVE = 0b1111'1111'1111'1111;


	void SortRelations (
		/* OUT */ const u16& relationsLookUpTableSize,
		/* OUT */ u16*& relationsLookUpTable
	) {
		u16 swapRelation;

		// bubble sort
		for (u16 iRelation = 1; iRelation < relationsLookUpTableSize; ++iRelation) {
			auto& curr = relationsLookUpTable[iRelation];
				
			for (u16 jRelation = 0; jRelation < iRelation; ++jRelation) {
				auto& prev = relationsLookUpTable[jRelation];

				if (curr < prev) {
					swapRelation = curr;
					for (u16 kRelation = iRelation; kRelation > jRelation; --kRelation) {
						relationsLookUpTable[kRelation] = relationsLookUpTable[kRelation - 1];
					}
					prev = swapRelation;
					break;
				}
			}
		}

		// move transform-only down
		u16 transfromOnlyCount = 0;
		u16 firstTransfromOnly = 0;

		for (u16 iRelation = 0; iRelation < relationsLookUpTableSize; ++iRelation) {
			if (relationsLookUpTable[iRelation] == NOT_REPRESENTIVE) {
				firstTransfromOnly = iRelation;
				break;
			}
		}

		transfromOnlyCount = relationsLookUpTableSize - firstTransfromOnly;
		for (u16 iRelation = firstTransfromOnly; iRelation != 0; --iRelation) {
			relationsLookUpTable[iRelation + transfromOnlyCount - 1] = relationsLookUpTable[iRelation - 1];
		}
		for (u16 iRelation = 0; iRelation < transfromOnlyCount; ++iRelation) {
			relationsLookUpTable[iRelation] = NOT_REPRESENTIVE;
		}
	}


	// LoopUp if a relation exsists if doesn't add one.
	//  Atfer recursive func execusion using 'mmRelationsLookUpTableSize' calculate final buffor size.
	//
	void CheckAddRelation (
		/* OUT */ u16& mmRelationsLookUpTableSize,
		/* OUT */ u16& mmRelationsLookUpTableCounter,
		/* OUT */ u16*& mmRelationsLookUpTable,
		/* IN  */ const u8& materialId,
		/* IN  */ const u8& meshId
	) {
		// ! This code definetly can be optimized further !

		// Relation consists of u8 material and u8 mesh. 
		u16 relation = (materialId << 8) + meshId;
		u8 isExisting = 0;

		for (u16 iRelation = 0; iRelation < mmRelationsLookUpTableSize; ++iRelation) {
			isExisting = (mmRelationsLookUpTable[iRelation] == relation);
			if (isExisting) break;
		}

		// Count Non-Duplicates
		if (!isExisting) ++mmRelationsLookUpTableSize;
		// But Add every relation including Duplicates for use later.
		mmRelationsLookUpTable[mmRelationsLookUpTableCounter] = relation;
		++mmRelationsLookUpTableCounter;
		//DEBUG spdlog::info ("R: {0:b}, IE: {1}", relation, isExisting);
	}


	void AddEmptyRelation (
		/* OUT */ u16& mmRelationsLookUpTableCounter,
		/* OUT */ u16*& mmRelationsLookUpTable,
		/* IN  */ const u8& materialId,
		/* IN  */ const u8& meshId
	) {
		u16 relation = (materialId << 8) + meshId;
		mmRelationsLookUpTable[mmRelationsLookUpTableCounter] = relation;
		++mmRelationsLookUpTableCounter;
	}

}


namespace RESOURCES::SCENE {

		void NodeCreate (
			/* IN  */ Json& parent,
			/* IN  */ const u8& materialsCount,
			/* IN  */ const u8& meshesCount,
			//
			/* OUT */ u16& mmRelationsLookUpTableSize,
			/* OUT */ u16& mmRelationsLookUpTableCounter,
			/* OUT */ u16*& mmRelationsLookUpTable,
			/* OUT */ u8& meshTableBytes,
			//
			/* OUT */ u16& parenthoodsCount,
			/* OUT */ u16& transformsCount
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
			
				if (materialId > materialsCount) { // VALIDATION
					DEBUG spdlog::error ("Selected invalid 'Material': {0}", materialId);
					exit (1);
				}

				if (meshId > meshesCount) {	// VALIDATION
					DEBUG spdlog::error ("Selected invalid 'Mesh': {0}", meshId);
					exit (1);
				}

				RELATION::CheckAddRelation (
					mmRelationsLookUpTableSize,
					mmRelationsLookUpTableCounter,
					mmRelationsLookUpTable,
					materialId, meshId
				);
			}

			DEBUG if ( parent.contains (D_MESH) ) {
				auto& nodeMesh = parent[D_MESH];
				meshId = nodeMesh.get<int> ();

				if (materialId > materialsCount) {
					DEBUG spdlog::error ("Selected invalid 'Debug Material': {0}", materialId);
					exit (1);
				}

				if (meshId > meshesCount) {
					DEBUG spdlog::error ("Selected invalid 'Debug Mesh': {0}", meshId);
					exit (1);
				}

				RELATION::CheckAddRelation (
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
					RELATION::AddEmptyRelation (
						mmRelationsLookUpTableCounter, mmRelationsLookUpTable,
						materialId, meshId
					);
				}
			}
            
			if ( parent.contains (CHILDREN) ) {
				auto& nodeChildren = parent[CHILDREN];
				auto childrenCount = nodeChildren.size ();

				// Ensure we add a parenthood only when there are defined elements in children node.
				parenthoodsCount += (childrenCount > 0);

				for (u8 iChild = 0; iChild < childrenCount; ++iChild) {
					auto& nodeChild = nodeChildren[iChild];

					NodeCreate (
						nodeChild, materialsCount, meshesCount, 
						mmRelationsLookUpTableSize, mmRelationsLookUpTableCounter, mmRelationsLookUpTable, meshTableBytes, 
						parenthoodsCount, transformsCount
					);
				}
			}

		}

		void Create (
			Json& json,
			//
			/* IN  */ const u8& materialIds,
			/* IN  */ const u8& mesheIds,
			//
			/* OUT */ u8*& meshTable,
			/* OUT */ u16*& relationsLookUpTable,
			//
			/* OUT */ u16& parenthoodsCount,
			/* OUT */ u16& transformsCount
		) {
			
			ZoneScopedN("RESOURCES::SCENE: Create");
			DEBUG { spdlog::info ("JSON Scene Initialization"); }

			std::ifstream file;
			file.open ( "res/data/scene.json" );
			file >> json; // Parse the file.	

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

			relationsLookUpTable = (u16*) malloc (materialIds * mesheIds * sizeof (u16));
			//relationsLookUpTable = (u16*) calloc (materialIds * mesheIds, sizeof (u16));
			u16 relationsLookUpTableNonDuplicates = 0; // Size and capacity is different !
			u16 relationsLookUpTableCounter = 0;

			auto& nodeRoot = json;
			NodeCreate (
				nodeRoot, materialIds, mesheIds, 
				relationsLookUpTableNonDuplicates, relationsLookUpTableCounter, relationsLookUpTable, meshTableBytes, 
				parenthoodsCount, transformsCount
			);

			meshTableBytes += relationsLookUpTableNonDuplicates * 2;
			//DEBUG spdlog::info ("mtb2: {0}", meshTableBytes);
			//DEBUG spdlog::info ("r: {0}", relationsLookUpTableNonDuplicates);

			// Allocate memory
			meshTable = (u8*) calloc (meshTableBytes, sizeof (u8));

			// It is necessery for load function to have relations now sorted
			//  so based on that list we can sort transforms.
			//const auto relationsSize = transformsCount - 1;
			RELATION::SortRelations (transformsCount, relationsLookUpTable);

			//DEBUG for (u8 i = 0; i < transformsCount; ++i) { // minus root transfrom
			//	spdlog::info ("{0}: {1:b}", i, relationsLookUpTable[i]);
			//}
		}


		
		void NodeLoad (
			/* IN  */ Json& parent,
			/* IN  */ u16*& relationsLookUpTable,
			// COMPONENTS
			/* OUT */ u8& childCounter, 
			/* OUT */ PARENTHOOD::Parenthood* parenthoods, 
			/* OUT */ u16& transformsCounter, 
			/* OUT */ TRANSFORM::LTransform* transforms
		) {
			u8 materialId = MATERIAL_INVALID;
			u8 meshId = MESH_INVALID;
			
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

				// Initialize for simplicity for now.
				TRANSFORM::LTransform tempTransform { 0 }; 

				{ // READ 
					r32* transform = (r32*) (void*) &(tempTransform.local);
					for (u8 iNode = 0; iNode < 3; ++iNode) {
						auto& node = nodeTransform[TRANSFORM_NAMES[iNode]];
						for (u8 iValue = 0; iValue < 3; ++iValue) {
							auto& value = node[iValue];
							transform[iNode * 3 + iValue] = value.get<float> ();
						}
					}
				}
				
				// porównywać relacje tego z poprzednimi elementami?
				// relacje muszą wtedy zawierać duplikaty...
				u16 relation = (materialId << 8) + meshId;

				u16 iTransform = 0; // FIND FIRST OCCURANCE OF SUCH A RELATION
				for (; relationsLookUpTable[iTransform] != relation; ++iTransform);
				// IF it's already set look for next spot.
				u16 jTransform = iTransform; // HACK!!! we assume scale is always non 0.
				for (; transforms[jTransform].local.scale.x != 0; ++jTransform);
				// FINALLY SET
				// First make sure light mesh doesn't render on release build.
				transforms[jTransform].local = tempTransform.local;

				// UNCOMMENT THIS WHEN READY
				transforms[jTransform].id = transformsCounter;
				++transformsCounter;

				// Now I need to set up Parenthoods correctly
				//  Which is When a node has children we assign
				//  to an unused parenthood 
				//  parent value -> transformsCounter
				//  child value -> child's transfromsCounter
				// Also Systems->GetFast have to be changed to GetSlow!
				//  No wait. if GameObjectID is connected to transfroms then theres an easier / better way to write that.
				
				//spdlog::info ("bc: {0}", childCounter);

				// UNCOMMENT THIS WHEN READY (ROOT CANNOT SET ITSELF AS A CHILD !)
				auto& currParenthood = parenthoods[0];
				currParenthood.base.children[childCounter] = transformsCounter - 1;
				++childCounter;

				//spdlog::info ("cc: {0}", childCounter);
			}
            
			if ( parent.contains (CHILDREN) ) {
				auto& nodeChildren = parent[CHILDREN];
				auto childrenCount = nodeChildren.size ();

				auto currParenthood = parenthoods + 1;
				currParenthood[0].id = transformsCounter - 1;
				u8 childchildrenCounter = 0;

				for (u8 iChild = 0; iChild < childrenCount; ++iChild) {
					auto& nodeChild = nodeChildren[iChild];
					NodeLoad (
						nodeChild, relationsLookUpTable,
						childchildrenCounter, currParenthood, // So we would refer to the next one.
						transformsCounter, transforms
					);
				}
			}

		}


		void NodeRootLoad (
			/* IN  */ Json& parent,
			/* IN  */ u16*& relationsLookUpTable,
			// COMPONENTS
			/* OUT */ u8& childCounter, 
			/* OUT */ PARENTHOOD::Parenthood* parenthoods, 
			/* OUT */ u16& transformsCounter, 
			/* OUT */ TRANSFORM::LTransform* transforms
		) {
			u8 materialId = MATERIAL_INVALID;
			u8 meshId = MESH_INVALID;
			
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

				// Initialize for simplicity for now.
				TRANSFORM::LTransform tempTransform { 0 }; 

				{ // READ 
					r32* transform = (r32*) (void*) &(tempTransform.local);
					for (u8 iNode = 0; iNode < 3; ++iNode) {
						auto& node = nodeTransform[TRANSFORM_NAMES[iNode]];
						for (u8 iValue = 0; iValue < 3; ++iValue) {
							auto& value = node[iValue];
							transform[iNode * 3 + iValue] = value.get<float> ();
						}
					}
				}
				
				// porównywać relacje tego z poprzednimi elementami?
				// relacje muszą wtedy zawierać duplikaty...
				u16 relation = (materialId << 8) + meshId;

				u16 iTransform = 0; // FIND FIRST OCCURANCE OF SUCH A RELATION
				for (; relationsLookUpTable[iTransform] != relation; ++iTransform);
				// IF it's already set look for next spot.
				u16 jTransform = iTransform; // HACK!!! we assume scale is always non 0.
				for (; transforms[jTransform].local.scale.x != 0; ++jTransform);
				// FINALLY SET
				// First make sure light mesh doesn't render on release build.
				transforms[jTransform].local = tempTransform.local;

				// UNCOMMENT THIS WHEN READY
				transforms[jTransform].id = transformsCounter;
				++transformsCounter;

				// Now I need to set up Parenthoods correctly
				//  Which is When a node has children we assign
				//  to an unused parenthood 
				//  parent value -> transformsCounter
				//  child value -> child's transfromsCounter
				// Also Systems->GetFast have to be changed to GetSlow!
				//  No wait. if GameObjectID is connected to transfroms then theres an easier / better way to write that.
				
				// UNCOMMENT THIS WHEN READY (ROOT CANNOT SET ITSELF AS A CHILD !)
				//auto& currParenthood = parenthoods[0];
				//currParenthood.base.children[childCounter] = transformsCounter;
				//++childCounter;
			}
            
			if ( parent.contains (CHILDREN) ) {
				auto& nodeChildren = parent[CHILDREN];
				auto childrenCount = nodeChildren.size ();

				// ROOT
				parenthoods[0].id = 0;
				u8 childchildrenCounter = 0;

				for (u8 iChild = 0; iChild < childrenCount; ++iChild) {
					auto& nodeChild = nodeChildren[iChild];
					NodeLoad (
						nodeChild, relationsLookUpTable,
						childchildrenCounter, parenthoods, // So we would refer to the next one.
						transformsCounter, transforms
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
			/* IN  */ u16*& relationsLookUpTable,
			// COMPONENTS
			/* IN  */ const u16& parenthoodsCount, 
			/* OUT */ PARENTHOOD::Parenthood*& parenthoods, 
			/* IN  */ const u16& transformsCount, 
			/* OUT */ TRANSFORM::LTransform*& transforms
		) {

			// Sorting and Setting ( 2 ways )
			// 1. Create a temp array for these components.
			// 2. Add components and move them when adding.


			u8 rootChildrenCounter = 0;
			u16 transformsCounter = 0;

			auto& nodeRoot = json;
			NodeRootLoad ( 
				nodeRoot, relationsLookUpTable,
				rootChildrenCounter, parenthoods,
				transformsCounter, transforms
			);

			// Finally free relations "LUT"
			free (relationsLookUpTable);
		}

}