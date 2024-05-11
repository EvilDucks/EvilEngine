#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"

#ifdef PLATFORM_WINDOWS_TOKEN
#include "platform/win/loader.hpp"
#endif

namespace RENDER::SYSTEMS {

    // Utilizing global memory.
	glm::mat4 tempModel;
	u64 tempIndex;


    // It needs to be faster!
    //
    // PrecalculateGlobalTransforms:
    //  is 4 fors - 4 condition statements
    //
    // ApplyDirtyFlag:
    //  is 4 fors + 1 if - 5 condition statements
    //
    // Maybe theres a way to reimplement 'GetComponentFast' to 'GetComponents'.


    void PrecalculateGlobalTransforms (
		const u64& parenthoodsCount,
		PARENTHOOD::Parenthood* parenthoods,
		const u64& transformsCount,
		TRANSFORM::LTransform* lTransforms,
		TRANSFORM::GTransform* gTransforms
	) {
        ZoneScopedN("RENDER::SYSTEMS: PrecalculateGlobalTransforms");

		glm::mat4 localSpace;
		
		// Root is always 1.0f; One root per canvas/world/screen!
		gTransforms[0] = glm::mat4(1.0f);

		for (u64 i = 0; i < parenthoodsCount; ++i) {
			auto& componentParenthood = parenthoods[i];
			auto& parenthood = componentParenthood.base;
			auto& parent = componentParenthood.id;

			auto& parentGlobal = gTransforms[parent];
			//
			for (u64 j = 0; j < parenthood.childrenCount; ++j) {
				auto& child = parenthood.children[j];
				localSpace = parentGlobal;

				TRANSFORM::ApplyModel (localSpace, lTransforms[child].local);
				gTransforms[child] = localSpace;
			}
		}
	}


    void ApplyDirtyFlag (
		const u64& parenthoodsCount, 
		PARENTHOOD::Parenthood* parenthoods,
		const u64& transformsCount, 
		TRANSFORM::LTransform* lTransforms,
		TRANSFORM::GTransform* gTransforms
	) {
        ZoneScopedN("RENDER::SYSTEMS: ApplyDirtyFlag");

		for (u64 i = 0; i < parenthoodsCount; ++i) {

			auto& componentParenthood = parenthoods[i];
			auto& parenthood = componentParenthood.base;
			auto& parentId = componentParenthood.id;
			auto& parentGTransform = gTransforms[parentId];

			for (u64 j = 0; j < parenthood.childrenCount; ++j) {

				auto& childId = parenthood.children[j];
				auto& childLTransform = lTransforms[childId];
				auto& childGTransform = gTransforms[childId];

				if (childLTransform.flags == TRANSFORM::DIRTY) {
					// Each time copy from parent it's globalspace.
					tempModel = parentGTransform;
					TRANSFORM::ApplyModel (tempModel, childLTransform.local);
					childGTransform = tempModel;
					childLTransform.flags = TRANSFORM::NOT_DIRTY;
				}
			}
		}
	}

}