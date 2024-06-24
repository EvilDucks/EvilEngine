#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "resources/manager.hpp"
#include "resources/json.hpp"

// What are relations?
//  Relations are an array that holds information about read object material+mesh
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
//   We first use calloc instead of malloc on Transform components array to make a not claimed key.


// [ Example of mmRelations Table Structure  ]
// material  mesh
// 1111.1111.1111.1111 -> Trasform only. Parenthoods -> Nodes with children nodes. Lights etc...
// 0000.0000.0000.0000 -> An actual relation with mat:0, mes:0.
// 0000.0000.0000.0000 -> A duplicate relation.
// 0000.0000.0000.0001 -> Different Mesh.
// 0000.0001.0000.0001 -> Different Material.

namespace RESOURCES::MMRELATION {

	// Objects that don't have a Mesh or Material or both
	//  are marked with the following value as invalid.
	const u16 NOT_REPRESENTIVE = 0b1111'1111'1111'1111;
	const u8 VALID_RELATION = 6;

	// RANGES
	const u8 MATERIAL_INVALID = 255;
	const u8 MESH_INVALID = 255;

	const u16 MAX_NODES = 256;


	void Find (
		/* IN_OUT */ u16& validKeyPos,
		/* IN     */ TRANSFORM::LTransform* const& transforms,
		/* IN     */ u16* const&  relationsLookUpTable,
		/* IN     */ const u16& relation
	) {
		// FIND FIRST OCCURANCE OF SUCH MMRELATION
		u16 iTransform = 0; for (; relationsLookUpTable[iTransform] != relation; ++iTransform);

		// IF it's already set look for next spot. // HACK!!! we assume scale is always non 0.
		validKeyPos = iTransform; for (; transforms[validKeyPos].base.scale.x != 0; ++validKeyPos);
	}


	void PrefabSortRelations (
		/* OUT */ const u16& mmrlutc,
		/* OUT */ u16*& mmrlut,
		/* OUT */ u8*& plut
	) {
		u16 mmrSwap;
		u8 pSwap;

		// bubble sort

		for (u16 iRelation = 1; iRelation < mmrlutc; ++iRelation) {
			auto& mmrCurr = mmrlut[iRelation];
			auto& pCurr = plut[iRelation];
				
			for (u16 jRelation = 0; jRelation < iRelation; ++jRelation) {
				auto& mmrPrev = mmrlut[jRelation];
				auto& pPrev = plut[jRelation];

				if (mmrCurr < mmrPrev) {

					mmrSwap = mmrCurr;
					//pSwap = pCurr;

					for (u16 kRelation = iRelation; kRelation > jRelation; --kRelation) {
						mmrlut[kRelation] = mmrlut[kRelation - 1];
						//plut[kRelation] = plut[kRelation - 1];
					}

					mmrPrev = mmrSwap;
					//pPrev = pSwap;

					break;
				}
			}
		}

		// HACK. does that work properly?!
		//  check it with 2 and more transfrom only objects in a same file!

		// move transform-only down
		u16 transfromOnlyCount = 0;
		u16 firstTransfromOnly = 0;

		for (u16 iRelation = 0; iRelation < mmrlutc; ++iRelation) {
			if (mmrlut[iRelation] == NOT_REPRESENTIVE) {
				firstTransfromOnly = iRelation;
				break;
			}
		}

		transfromOnlyCount = mmrlutc - firstTransfromOnly;
		for (u16 iRelation = firstTransfromOnly; iRelation != 0; --iRelation) {
			mmrlut[iRelation + transfromOnlyCount - 1] = mmrlut[iRelation - 1];
		}

		for (u16 iRelation = 0; iRelation < transfromOnlyCount; ++iRelation) {
			mmrlut[iRelation] = NOT_REPRESENTIVE;
		}
	}



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

		// HACK. does that work properly?!
		//  check it with 2 and more transfrom only objects in a same file!

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
		/* OUT */ u16& mmRelationsLookUpTableUniques,
		/* OUT */ u16& mmRelationsLookUpTableCounter,
		/* OUT */ u16*& mmRelationsLookUpTable,
		/* IN  */ const u8& materialId,
		/* IN  */ const u8& meshId
	) {
		// ! This code definetly can be optimized !

		// Relation consists of u8 material and u8 mesh. 
		u16 relation = (materialId << 8) + meshId;
		u8 isExisting = 0;

		for (u16 iRelation = 0; iRelation < mmRelationsLookUpTableCounter; ++iRelation) {
			isExisting = (mmRelationsLookUpTable[iRelation] == relation);
			if (isExisting) break;
		}

		// Count Non-Duplicates
		if (!isExisting) ++mmRelationsLookUpTableUniques;

		// But Add every relation including Duplicates for use later.
		mmRelationsLookUpTable[mmRelationsLookUpTableCounter] = relation;
		++mmRelationsLookUpTableCounter;
		//DEBUG spdlog::info ("R: {0:b}, IE: {1}", relation, isExisting);
	}


	// Empty refers to as non representable - does not have a material nor mesh.
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


	void Log (
		/* IN_OUT */ const u16& mmrlutu,
		/* IN_OUT */ const u16& mmrlutc,
		/* IN_OUT */ u16* const& mmrlut
	) {
		DEBUG spdlog::info ("s: {0}, u: {1}", mmrlutc, mmrlutu);
		for (u8 i = 0; i < mmrlutc; ++i) {
			DEBUG spdlog::info ("r: {0:08b}'{1:08b}", (u8)(mmrlut[i] >> 8), (u8)(mmrlut[i]));
		}
	}

}


namespace RESOURCES::MMRELATION {

	// THIS CODE NEED REFACTORING !
	void SetMeshTableValue (
		/* OUT */ u8*& meshTable,
		/* IN  */ u16* const& relationsLookUpTable,
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

}