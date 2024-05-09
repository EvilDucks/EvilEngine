#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"

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

		const char* D_MATERIAL = "d_material";
		const char* D_MESH = "d_mesh";

		u8 MATERIAL_INVALID = 255;


		// LoopUp if a relation exsists if doesn't add one.
		//  Atfer recursive func execusion using 'mmRelationsLookUpTableSize' calculate final buffor size.
		//
		void CheckAddRelation (
			/* OUT */ u16& mmRelationsLookUpTableSize,
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

			if (!isExisting) { // push_back
				mmRelationsLookUpTable[mmRelationsLookUpTableSize] = relation;
				++mmRelationsLookUpTableSize;
			}

			//DEBUG spdlog::info ("R: {0:b}, IE: {1}", relation, isExisting);
		}


		void NodeCreate (
			/* IN  */ Json& parent,
			/* IN  */ const u8& materialsCount,
			/* IN  */ const u8& meshesCount,
			//
			/* OUT */ u16& mmRelationsLookUpTableSize,
			/* OUT */ u16*& mmRelationsLookUpTable,
			/* OUT */ u8& meshTableBytes,
			//
			/* OUT */ u16& parenthoodsCount,
			/* OUT */ u16& transformsCount
		) {

			u8 materialId = MATERIAL_INVALID;
			
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

			if ( parent.contains (MESH) ) {
				auto& nodeMesh = parent[MESH];

				if (materialId < materialsCount) { // VALIDATION

					u8 meshId = nodeMesh.get<int> ();

					if (meshId < meshesCount) {	// VALIDATION

						CheckAddRelation (
							mmRelationsLookUpTableSize,
							mmRelationsLookUpTable,
							materialId,
							meshId
						);

					} else {
						DEBUG spdlog::error ("Selected invalid 'Mesh': {0}", meshId);
						exit (1);
					}

				} else {
					DEBUG spdlog::error ("Selected invalid 'Material': {0}", materialId);
					exit (1);
				}
			}

			if ( parent.contains (D_MESH) ) {
				auto& nodeMesh = parent[D_MESH];

				if (materialId < materialsCount) { // VALIDATION

					u8 meshId = nodeMesh.get<int> ();

					if (meshId < meshesCount) {	// VALIDATION

						CheckAddRelation (
							mmRelationsLookUpTableSize,
							mmRelationsLookUpTable,
							materialId,
							meshId
						);

					} else {
						DEBUG spdlog::error ("Selected invalid 'Debug Mesh': {0}", meshId);
						exit (1);
					}
					
				} else {
					DEBUG spdlog::error ("Selected invalid 'Debug Material': {0}", materialId);
					exit (1);
				}
			}

			if ( parent.contains (TRANSFORM) ) {
				auto& nodeTransform = parent[TRANSFORM];
				++transformsCount;
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
						mmRelationsLookUpTableSize, mmRelationsLookUpTable, meshTableBytes, 
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

			u16* relationsLookUpTable = (u16*) malloc (materialIds * mesheIds * sizeof (u16));
			u16 relationsLookUpTableSize = 0; // Size and capacity is different !

			auto& nodeRoot = json;
			NodeCreate (
				nodeRoot, materialIds, mesheIds, 
				relationsLookUpTableSize, relationsLookUpTable, meshTableBytes, 
				parenthoodsCount, transformsCount
			);

			meshTableBytes += relationsLookUpTableSize * 2;
			DEBUG spdlog::info ("mtb2: {0}", meshTableBytes);

			// Allocate memory
			meshTable = (u8*) calloc (meshTableBytes, sizeof (u8));

			// It might be use to 'Load' function...
			free (relationsLookUpTable);

		}


		void NodeLoad (
			/* IN  */ Json& parent
		) {
			
			if ( parent.contains (NAME) ) {
				auto& nodeName = parent[NAME];
			}

			if ( parent.contains (MATERIAL) ) {
				auto& nodeMaterial = parent[MATERIAL];
			}

			if ( parent.contains (TEXTURE1) ) {
				auto& nodeTexture1 = parent[TEXTURE1];
			}

			if ( parent.contains (MESH) ) {
				auto& nodeMesh = parent[MESH];
			}

			if ( parent.contains (D_MATERIAL) ) {
				auto& nodeDebugMaterial = parent[D_MATERIAL];
			}

			if ( parent.contains (D_MESH) ) {
				auto& nodeDebugMesh = parent[D_MESH];
			}

			if ( parent.contains (TRANSFORM) ) {
				auto& nodeTransform = parent[TRANSFORM];
			}
            
			if ( parent.contains (CHILDREN) ) {
				auto& nodeChildren = parent[CHILDREN];
				auto childrenCount = nodeChildren.size ();

				for (u8 iChild = 0; iChild < childrenCount; ++iChild) {
					auto& nodeChild = nodeChildren[iChild];
					NodeLoad (nodeChild);
				}
			}

		}


		void Load (
			/* IN  */ Json& json
		) {

		}

}