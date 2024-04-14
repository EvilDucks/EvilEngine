#pragma once
#include "tool/debug.hpp"
#include "gl.hpp"

#ifdef PLATFORM_WINDOWS_TOKEN
#include "platform/win/loader.hpp"
#endif

#include "scenes.hpp"

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
		TRANSFORM::Transform* transforms
	) {
		glm::mat4 localSpace;
		// Root is always 1.0f; One root per canvas/world/screen!
		transforms[0].global = glm::mat4(1.0f);

		//DEBUG spdlog::info ("a: {}", parenthoodsCount);
		//
		for (u64 i = 0; i < parenthoodsCount; ++i) {
			auto& componentParenthood = parenthoods[i];
			auto& parenthood = componentParenthood.base;
			auto& parent = componentParenthood.id;
			//
			u64 transformIndex = OBJECT::ID_DEFAULT;
			//
			OBJECT::GetComponentFast<TRANSFORM::Transform> (
				transformIndex, transformsCount, transforms, parent
			);
			//
			auto& parentGlobal = transforms[transformIndex].global;
			//
			for (u64 j = 0; j < parenthood.childrenCount; ++j) {
				auto& child = parenthood.children[j];
				//
				OBJECT::GetComponentFast<TRANSFORM::Transform> (
					transformIndex, transformsCount, transforms, child
				);
				//
				auto& childTransform = transforms[transformIndex];
				// Each time copy from parent it's globalspace.
				localSpace = parentGlobal; 
				//
				TRANSFORM::ApplyModel (localSpace, childTransform.local);
				childTransform.global = localSpace;
			}
		}
	}


    void ApplyDirtyFlag (
		const u64& parenthoodsCount, 
		PARENTHOOD::Parenthood* parenthoods,
		const u64& transformsCount, 
		TRANSFORM::Transform* transforms
	) {
		for (u64 i = 0; i < parenthoodsCount; ++i) {
			auto& componentParenthood = parenthoods[i];
			auto& parenthood = componentParenthood.base;
			auto& parentId = componentParenthood.id;
			//
			tempIndex = OBJECT::ID_DEFAULT;
			//
			OBJECT::GetComponentFast<TRANSFORM::Transform> (
				tempIndex, transformsCount, transforms, parentId
			);
			//
			auto& parentTransform = transforms[tempIndex];
			//
			for (u64 j = 0; j < parenthood.childrenCount; ++j) {
				auto& childId = parenthood.children[j];
				//
				OBJECT::GetComponentFast<TRANSFORM::Transform> (
					tempIndex, transformsCount, transforms, childId
				);
				//
				auto& childTransform = transforms[tempIndex];
				if (childTransform.flags == TRANSFORM::DIRTY) {
					// Each time copy from parent it's globalspace.
					tempModel = parentTransform.global;
					TRANSFORM::ApplyModel (tempModel, childTransform.local);
					childTransform.global = tempModel;
					childTransform.flags = TRANSFORM::NOT_DIRTY;
				}
			}
		}
	}

}