#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

#include "util/mmrelation.hpp"

// The Rule Book
// 1. Whats in ROOT-Hierarchy has to have a Transfrom component.
// 2. GameObjectID is an Transform's Component Array id extension
//  Meaning [ 0,1,2 - Transfrom only, 3,4,5 - T & MA & ME, 6,7,8 - NO Transform ]
//  This simplifies Transfrom search by a lot!
//

// 1. Creation Phase
// - Allocate memory for meshTable 
// - Allocate memory for All other Components!
//  ! Parenthood is the worst for the moment as its structure holds a pointer to it's children definition.
//

// 2. Loading Phase
//  Differentiate ROOT and not-ROOT function calls.
//  - Root sets itself (not the next Parenthood component) as a parent of it's children.
//  - Root does not add itself as a child to a previous Parenthood component.
//

namespace RESOURCES::SCENE {

	// VIEWS
	const char* WORLD 					= "world";
	const char* CANVAS 					= "canvas";

	// COMPONENTS
	const char* NAME 					= "name";
	const char* CHILDREN 				= "children";
	const char* MATERIAL 				= "material";
	const char* TEXTURE1 				= "texture1";
	const char* MESH 					= "mesh";
	const char* ROTATING 				= "rotating";
	const char* RIGIDBODY				= "rigidbody";
	const char* PLAYER	 				= "player";

	const char* COLLIDER 				= "collider";
	const char* COLLIDER_TYPES 			= "groups";
	const char* COLLIDER_TYPE_MAP		= "map";
	const char* COLLIDER_TYPE_TRIGGER	= "trigger";
	const char* COLLIDER_TYPE_PLAYER	= "player";

	const u8 COLLIDER_GROUPS_COUNT 		= 3;
	const char* COLLIDER_GROUPS[COLLIDER_GROUPS_COUNT] { 
		COLLIDER_TYPE_MAP, COLLIDER_TYPE_TRIGGER, COLLIDER_TYPE_PLAYER 
	};


	const char* D_MATERIAL				= "d_material";
	const char* D_MESH					= "d_mesh";

	const char* TRANSFORM				= "transform";
	const char* POSITION				= "position";
	const char* ROTATION				= "rotation";
	const char* SCALE					= "scale";

	const char* TRANSFORM_NAMES [3] { POSITION, ROTATION, SCALE };

}


namespace RESOURCES::SCENE::COMPONENTS {

	void Transform (
		/* IN     */ Json& node,
		/* IN     */ u16*& relationsLookUpTable,
		/* OUT    */ TRANSFORM::LTransform* transforms,
		/* OUT    */ u16& transformsCounter, 
		/* IN     */ const u16& relation,
		/* IN_OUT */ u16& validKeyPos
	) {
		auto& nodeTransform = node[TRANSFORM];

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
			
		MMRELATION::Find (validKeyPos, transforms, relationsLookUpTable, relation);

		// FINALLY SET
		transforms[validKeyPos].base = tempTransform.base;
		transforms[validKeyPos].id = transformsCounter;
		++transformsCounter;
	}

}


namespace RESOURCES::SCENE::NODE {

	void Create (
		/* IN  */ Json& 		node,
		/* IN  */ const u8& 	materialsCount,
		/* IN  */ const u8& 	meshesCount,
		//
		/* OUT */ u16& 			mmRelationsLookUpTableSize,
		/* OUT */ u16& 			mmRelationsLookUpTableCounter,
		/* OUT */ u16*& 		mmRelationsLookUpTable,
		/* OUT */ u16& 			relationsLookUpTableOffset,
		/* OUT */ u8& 			meshTableBytes,
		// -> COMPONENTS
		/* OUT */ u16& 			parenthoodsCount,
		/* OUT */ u16& 			childrenSumCount,
		/* OUT */ u16& 			transformsCount,
		/* OUT */ u16& 			rotatingsCount,
		/* OUT */ u16& 			collidersMapCount,
		/* OUT */ u16& 			collidersTriggerCount,
		/* OUT */ u16& 			collidersPlayerCount,
		/* OUT */ u16& 			rigidbodiesCount,
		/* OUT */ u16& 			playersCount
	) {
		u8 materialId = RESOURCES::MMRELATION::MATERIAL_INVALID;
		u8 meshId = RESOURCES::MMRELATION::MESH_INVALID;

		u8 isMaterial 		= node.contains (MATERIAL);
		u8 isMesh 			= node.contains (MESH);
		u8 isChildren 		= node.contains (CHILDREN);
		u8 isTexture1 		= node.contains (TEXTURE1);
		u8 isTransform		= node.contains (TRANSFORM);
		u8 isRotating 		= node.contains (ROTATING);
		u8 isCollider 		= node.contains (COLLIDER);
		u8 isRigidbody		= node.contains (RIGIDBODY);
		u8 isPlayer 		= node.contains (PLAYER);

		u8 isValidRenderable = 0;
		isValidRenderable += (isMaterial	<< 1);
		isValidRenderable += (isMesh		<< 2);

		if ( isMaterial ) {
			auto& nodeMaterial = node[MATERIAL];
			materialId = (u8)(nodeMaterial.get<int> ());
		}

		if ( isTexture1 ) {
			auto& nodeTexture1 = node[TEXTURE1];
		}

		if ( isMesh ) {
			auto& nodeMesh = node[MESH];
			meshId = (u8)(nodeMesh.get<int> ());
		
			if (materialId > materialsCount) ErrorExit ("Selected invalid 'Material': {0}", materialId) // VALIDATION
			if (meshId > meshesCount) ErrorExit ("Selected invalid 'Mesh': {0}", meshId) // VALIDATION
		}

		DEBUG { // We do want to override release components with debug components! 
			u8 isName 		= node.contains (NAME);
			u8 isDMaterial 	= node.contains (D_MATERIAL);
			u8 isDMesh		= node.contains (D_MESH);

			isValidRenderable += (((isMaterial	== 0) & isDMaterial)	<< 1);
			isValidRenderable += (((isMesh		== 0) & isDMesh)		<< 2);

			if ( isName ) {
				auto& nodeName = node[NAME];
			}

			if ( isDMaterial ) {
				auto& nodeMaterial = node[D_MATERIAL];
				materialId = (u8)(nodeMaterial.get<int> ());
			}

			if ( isDMesh ) {
				auto& nodeMesh = node[D_MESH];
				meshId = nodeMesh.get<int> ();

				if (materialId > materialsCount) ErrorExit ("Selected invalid 'Debug Material': {0}", materialId);
				if (meshId > meshesCount) ErrorExit("Selected invalid 'Debug Mesh': {0}", meshId);
			}
		}

		if (isValidRenderable == RESOURCES::MMRELATION::VALID_RELATION) {

			MMRELATION::CheckAddRelation (
				mmRelationsLookUpTableSize,
				mmRelationsLookUpTableCounter,
				mmRelationsLookUpTable,
				materialId, meshId
			);

		} else {

			MMRELATION::AddEmptyRelation (
				mmRelationsLookUpTableCounter, mmRelationsLookUpTable,
				materialId, meshId
			);

			++relationsLookUpTableOffset;
		}

		if ( isTransform )	++transformsCount;
		if ( isRotating )	++rotatingsCount;

		if ( isCollider )	{
			auto& collider = node[COLLIDER];

			if (collider.contains(COLLIDER_TYPES)) {
				auto& group = collider[COLLIDER_TYPES];

				// This is a lot. Prob. can be written a lot better.
				u16* collidersCount[COLLIDER_GROUPS_COUNT] {
					&collidersMapCount,
					&collidersTriggerCount,
					&collidersPlayerCount,
				};

				for (u8 i = 0; i < group.size(); ++i) {
					std::string key = group[i].get<std::string>();
					for (u8 iComperable = 0; iComperable < COLLIDER_GROUPS_COUNT; ++iComperable) {
						// Compare - is a bunch of if statements together so its optimal to do an if statement here and break.
						if (key.compare(COLLIDER_GROUPS[iComperable]) == 0) {
							++(*collidersCount[iComperable]);
							break;
						}
					}
				}

			} else DEBUG ErrorExit ("Object with Collider does not specify it's group!");
		}

		if ( isRigidbody )	++rigidbodiesCount;
		if ( isPlayer )		++playersCount;
            
		if ( isChildren ) {
			auto& nodeChildren = node[CHILDREN];
			auto childrenCount = nodeChildren.size ();

			// Ensure we add a parenthood only when there are defined elements in children node.
			parenthoodsCount += (childrenCount > 0);

			// Get bytes for Parenthood's children allocation.
			childrenSumCount += childrenCount;

			for (u8 iChild = 0; iChild < childrenCount; ++iChild) {
				auto& nodeChild = nodeChildren[iChild];

				Create (
					nodeChild, materialsCount, meshesCount, 
					mmRelationsLookUpTableSize, mmRelationsLookUpTableCounter, mmRelationsLookUpTable, relationsLookUpTableOffset,
					meshTableBytes, 
					parenthoodsCount, childrenSumCount, transformsCount, rotatingsCount, 
					collidersMapCount, collidersTriggerCount, collidersPlayerCount, 
					rigidbodiesCount, playersCount
				);
			}
		}
	}

	template <bool isRoot>
	void Load (
		/* IN  */ Json& 					node,						// REF
		/* OUT */ u16* 						childrenTable,				// CPY
		/* IN  */ u16*& 					relationsLookUpTable,		// REF
		/* IN  */ const u16& 				relationsLookUpTableOffset,	// REF
		/* OUT */ u8*& 						meshTable,					// REF
		/* OUT */ u8& 						childCounter, 				// REF
		/* OUT */ PARENTHOOD::Parenthood* 	parenthoods, 				// CPY
		/* OUT */ u16& 						transformsCounter, 			// REF
		/* OUT */ TRANSFORM::LTransform* 	transforms,					// CPY
		/* OUT */ u16& 						rotatingsCounter, 			// REF
		/* OUT */ ROTATING::Rotating*& 		rotatings					// REF
	) {
		const u8 VALID_RENDERABLE = 0b0000'0111;

		u8 materialId = RESOURCES::MMRELATION::MATERIAL_INVALID;
		u8 meshId = RESOURCES::MMRELATION::MESH_INVALID;

		u16 validKeyPos = MMRELATION::NOT_REPRESENTIVE;					// This Object Transform index and GameObject id.

		u8 isMaterial 		= node.contains (MATERIAL);
		u8 isTexture1 		= node.contains (TEXTURE1);
		u8 isMesh 			= node.contains (MESH);
		u8 isTransform		= node.contains (TRANSFORM);
		u8 isRotating 		= node.contains (ROTATING);
		u8 isChildren 		= node.contains (CHILDREN);

		u8 isValidRenderable = 0;
		isValidRenderable += (isTransform	<< 0);
		isValidRenderable += (isMaterial	<< 1);
		isValidRenderable += (isMesh		<< 2);

		if ( isMaterial ) {
			auto& nodeMaterial = node[MATERIAL];
			materialId = nodeMaterial.get<int> ();
		}

		if ( isTexture1 ) {
			auto& nodeTexture1 = node[TEXTURE1];
		}

		if ( isMesh ) {
			auto& nodeMesh = node[MESH];
			meshId = nodeMesh.get<int> ();
		}

		DEBUG { // We do want to override release components with debug components! 
			u8 isName 		= node.contains (NAME);
			u8 isDMaterial 	= node.contains (D_MATERIAL);
			u8 isDMesh		= node.contains (D_MESH);

			isValidRenderable += (((isMaterial	== 0) & isDMaterial)	<< 1);
			isValidRenderable += (((isMesh		== 0) & isDMesh)		<< 2);

			if ( isDMaterial ) {
				auto& nodeMaterial = node[D_MATERIAL];
				materialId = nodeMaterial.get<int> ();
			}

			if ( isDMesh ) {
				auto& nodeMesh = node[D_MESH];
				meshId = nodeMesh.get<int> ();
			}

			if ( isName ) { auto& nodeName = node[NAME]; }

			if ( isRotating + (isTransform << 1) == 1 ) 				// VALIDATION
				ErrorExit ("Object with Rotation component does not possess Transform!");
		}

		if ( isValidRenderable == VALID_RENDERABLE ) {
			MMRELATION::SetMeshTableValue (meshTable, relationsLookUpTable, relationsLookUpTableOffset, materialId, meshId);
		}

		if ( isTransform ) {
			u16 relation = (materialId << 8) + meshId;

			COMPONENTS::Transform (node, relationsLookUpTable, transforms, transformsCounter, relation, validKeyPos);

			if constexpr (!isRoot) { 									// Add this objectID to parenthood as a childID.
				auto& currParenthood = parenthoods[0];
				currParenthood.base.children[childCounter] = validKeyPos;
				++childCounter;
			}
		}

		if ( isRotating ) {
			auto& nodeRotating = node[ROTATING];
			auto& rotating = rotatings[rotatingsCounter].base;

			// READ
			for (u8 iValue = 0; iValue < 3; ++iValue) {
				auto& value = nodeRotating[iValue];
				rotating[iValue] = value.get<float> ();
			}

			rotatings[rotatingsCounter].id = validKeyPos;
			++rotatingsCounter;
		}
            
		if ( isChildren ) {
			auto& nodeChildren = node[CHILDREN];
			auto childrenCount = nodeChildren.size ();

			if constexpr (isRoot) {										
				parenthoods->id = 0;									// Prep it's necessery values.
				
			} else {
				++parenthoods;											// Cascade into next parenthoot component.
				parenthoods->id = validKeyPos;							// Prep it's necessery values.
			}

			parenthoods->base.childrenCount = childrenCount;			// Prep it's necessery values.
			parenthoods->base.children = childrenTable;
			
			childrenTable += childrenCount; 							// 'equals' So we don't overlap parenthood trees with each child children.
			u8 childchildrenCounter = 0; 								// Create a new counter (remember recursive!)

			for (u8 iChild = 0; iChild < childrenCount; ++iChild) {
				auto& nodeChild = nodeChildren[iChild];
				Load<false> (
					nodeChild, childrenTable, 
					relationsLookUpTable, relationsLookUpTableOffset,
					meshTable,
					childchildrenCounter, parenthoods,
					transformsCounter, transforms,
					rotatingsCounter, rotatings
				);
			}
		}
	}

}


namespace RESOURCES::SCENE {

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
		/* OUT */ u16& rotatingsCount,
		/* OUT */ u16& collidersMapCount,
		/* OUT */ u16& collidersTriggerCount,
		/* OUT */ u16& collidersPlayerCount,
		/* OUT */ u16& rigidbodiesCount,
		/* OUT */ u16& playersCount
	) {
		PROFILER { ZoneScopedN("RESOURCES::SCENE: Create"); }

		// We initialize it with 1 because theres 1 byte representing materials count.
		// And theres a byte for each material to represent how many different meshes to render it has.
		u8 meshTableBytes = 1 + materialIds;
			
		relationsLookUpTable = (u16*) malloc (MMRELATION::MAX_NODES * sizeof (u16));
		u16 relationsLookUpTableNonDuplicates = 0;
		u16 relationsLookUpTableCounter = 0;

		//  We'll allocate it with one call but make it point to different pointers later.
		u16 childrenSumCount = 0; 						

		DEBUG { // DEBUG only validation.
			auto& nodeWorld = json[WORLD];
			if ( json.contains (WORLD) || json[WORLD].size() != 0 ) {

				NODE::Create (
					nodeWorld, materialIds, mesheIds, 
					relationsLookUpTableNonDuplicates, relationsLookUpTableCounter, relationsLookUpTable, relationsLookUpTableOffset,
					meshTableBytes, 
					parenthoodsCount, childrenSumCount, transformsCount, rotatingsCount, 
					collidersMapCount, collidersTriggerCount, collidersPlayerCount, rigidbodiesCount, playersCount
				);

				if (relationsLookUpTableCounter > MMRELATION::MAX_NODES)
					ErrorExit (
						"Implementation doesn't support more then {0} nodes inside a scene/world, nodes:", 
						MMRELATION::MAX_NODES, relationsLookUpTableCounter
					);

			} else ErrorExit ("Scene does not contain a valid 'world' key!");

		} else {
			auto& nodeWorld = json[WORLD];

			NODE::Create (
				nodeWorld, materialIds, mesheIds, 
				relationsLookUpTableNonDuplicates, relationsLookUpTableCounter, relationsLookUpTable, relationsLookUpTableOffset,
				meshTableBytes, 
				parenthoodsCount, childrenSumCount, transformsCount, rotatingsCount, 
				collidersMapCount, collidersTriggerCount, collidersPlayerCount, rigidbodiesCount, playersCount
			);

		}

		//DEBUG { spdlog::info("c: {0}, p: {1}", collidersCount, playersCount); }

		meshTableBytes += (relationsLookUpTableNonDuplicates) * 2;

		// Allocate memory
		meshTable = (u8*) calloc (meshTableBytes, sizeof (u8));
		childrenTable = (u16*) malloc (childrenSumCount * sizeof (u16));

		MMRELATION::SortRelations (transformsCount, relationsLookUpTable);	
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

		NODE::Load<true> (
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
