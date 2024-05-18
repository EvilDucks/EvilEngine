#pragma once
#include "tool/debug.hpp"
#include "render/gl.hpp"

namespace TRANSFORM {

	using Position		= glm::vec3;
	using Rotation		= glm::vec3;
	using Scale			= glm::vec3;

	using GTransform	= glm::mat4; // 16 x 4b as it is r32

	const u8 NOT_DIRTY = 0;
	const u8 DIRTY = 1;

	struct Base {
		Position position;
		Rotation rotation;
		Scale scale;
	};

	struct LTransform {
		GameObjectID id = 0;				//
		Base local;							// 9 * 4b
		u8 flags;							// 1b
	};


	void ApplyModel (glm::mat4& model, const Base& local) {
		model = glm::translate	(model, local.position);
		model = glm::rotate		(model, glm::radians (local.rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
		model = glm::rotate		(model, glm::radians (local.rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
		model = glm::rotate		(model, glm::radians (local.rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
		model = glm::scale		(model, local.scale);
	}

}


#include "parenthood.hpp"

namespace TRANSFORM {

	// Utilizing global memory.
	glm::mat4 tempTransform;

	void Precalculate (
		const u64& parenthoodsCount,
		PARENTHOOD::Parenthood* parenthoods,
		const u64& transformsCount,
		TRANSFORM::LTransform* lTransforms,
		TRANSFORM::GTransform* gTransforms
	) {
        PROFILER { ZoneScopedN ("TRANSFORM:Precalculate"); }

		glm::mat4 localSpace;
		// ROOT
		gTransforms[0] = glm::mat4(1.0f);
		TRANSFORM::ApplyModel (gTransforms[0], lTransforms[0].local);

		for (u64 i = 0; i < parenthoodsCount; ++i) {
			auto& componentParenthood = parenthoods[i];
			auto& parenthood = componentParenthood.base;
			auto& parent = componentParenthood.id;
			auto& parentGlobal = gTransforms[parent];

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
        PROFILER { ZoneScopedN ("TRANSFROM:ApplyDirtyFlag"); }

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
					tempTransform = parentGTransform;
					TRANSFORM::ApplyModel (tempTransform, childLTransform.local);
					childGTransform = tempTransform;
					childLTransform.flags = TRANSFORM::NOT_DIRTY;
				}
			}
		}
	}

}