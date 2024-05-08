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
		//transforms[0].global = glm::mat4(1.0f);
		gTransforms[0] = glm::mat4(1.0f);

		//DEBUG spdlog::info ("a: {}", parenthoodsCount);
		//
		for (u64 i = 0; i < parenthoodsCount; ++i) {
			auto& componentParenthood = parenthoods[i];
			auto& parenthood = componentParenthood.base;
			auto& parent = componentParenthood.id;
			//
			u64 transformIndex = OBJECT::ID_DEFAULT;
			//
			OBJECT::GetComponentFast<TRANSFORM::LTransform> (
				transformIndex, transformsCount, lTransforms, parent
			);
			//DEBUG spdlog::info ("a: {0}", transformIndex);
			//
			//auto& parentGlobal = transforms[transformIndex].global;
			auto& parentGlobal = gTransforms[transformIndex];
			//
			for (u64 j = 0; j < parenthood.childrenCount; ++j) {
				auto& child = parenthood.children[j];
				//DEBUG spdlog::info ("x: {0}, {1}", transforms[transformIndex].id, child);
				//
				OBJECT::GetComponentFast<TRANSFORM::LTransform> (
					transformIndex, transformsCount, lTransforms, child
				);
				//DEBUG spdlog::info ("b: {0}", transformIndex);
				//
				//auto& childTransform = transforms[transformIndex];
				// Each time copy from parent it's globalspace.
				localSpace = parentGlobal; 
				//
				//TRANSFORM::ApplyModel (localSpace, childTransform.local);
				//childTransform.global = localSpace;
				TRANSFORM::ApplyModel (localSpace, lTransforms[transformIndex].local);
				gTransforms[transformIndex] = localSpace;
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
			//
			tempIndex = OBJECT::ID_DEFAULT;
			//
			OBJECT::GetComponentFast<TRANSFORM::LTransform> (
				tempIndex, transformsCount, lTransforms, parentId
			);
			//
			auto& parentGTransform = gTransforms[tempIndex];
			//
			for (u64 j = 0; j < parenthood.childrenCount; ++j) {
				auto& childId = parenthood.children[j];
				//
				OBJECT::GetComponentFast<TRANSFORM::LTransform> (
					tempIndex, transformsCount, lTransforms, childId
				);
				//
				auto& childLTransform = lTransforms[tempIndex];
				auto& childGTransform = gTransforms[tempIndex];
				//
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