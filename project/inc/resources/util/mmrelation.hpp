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

	const u16 MAX_NODES = 256;

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
		// ! This code definetly can be optimized !

		// Relation consists of u8 material and u8 mesh. 
		u16 relation = (materialId << 8) + meshId;
		u8 isExisting = 0;

		for (u16 iRelation = 0; iRelation < mmRelationsLookUpTableCounter; ++iRelation) {
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