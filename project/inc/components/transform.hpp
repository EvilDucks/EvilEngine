#pragma once
#include "tool/debug.hpp"
#include "render/gl.hpp"

namespace TRANSFORM {

	using Position		= glm::vec3;
	using Rotation		= glm::vec3;
	using Scale			= glm::vec3;

	using GTransform	= glm::mat4; // 16 x 4b as it is r32

	//const u8 NOT_DIRTY = 0;
	const u8 DIRTY		= 0b0000'0001;
	//const u8 LAST_CHILD	= 0b0000'0010; // You are NOT_THE_FATHER :v

	struct Base {
		Position position;
		Rotation rotation;
		Scale scale;
	};

	struct LTransform {
		GameObjectID id = 0;				//
		Base base;							// 9 * 4b
		u8 flags;							// 1b
	};


	void ApplyModel (glm::mat4& model, const Base& transfrom) {
		model = glm::translate	(model, transfrom.position);
		model = glm::rotate		(model, glm::radians (transfrom.rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
		model = glm::rotate		(model, glm::radians (transfrom.rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
		model = glm::rotate		(model, glm::radians (transfrom.rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
		model = glm::scale		(model, transfrom.scale);
	}

}


#include <glm/gtx/matrix_decompose.hpp>

namespace TRANSFORM::TRANSFORMATION {

	void Decompose (
		/* IN  */ glm::mat4& matrix,
		/* OUT */ glm::vec3& scale,
		/* OUT */ glm::quat& rotation,
		/* OUT */ glm::vec3& translation,
		/* OUT */ glm::vec3& skew,
		/* OUT */ glm::vec4& perspective
	) {
		glm::decompose (matrix, scale, rotation, translation, skew, perspective);
		rotation = glm::conjugate(rotation);
	}

	void QuaternionToAxis (
		/* IN  */ glm::quat& qRotation, 
		/* OUT */ glm::vec3& eRotation
	) {
		//const float PI = 3.14159f;
		//eRotation = glm::eulerAngles(qRotation); // * PI / 180.f; // Convert from euler to radians
		eRotation = glm::degrees(glm::eulerAngles(qRotation));// * (180.f / PI);
	}

}


#include "parenthood.hpp"

namespace TRANSFORM {

	void Log (LTransform& transformComponent) {
		auto& transform = transformComponent.base;

		spdlog::info (
			"Transform: "
			"t: {0:03.2f}, {1:03.2f}, {2:03.2f} \t"
			"r: {3:03.2f}, {4:03.2f}, {5:03.2f} \t"
			"s: {6:03.2f}, {7:03.2f}, {8:03.2f} \t"
			"id: {9}",
			transform.position.x, transform.position.y, transform.position.z,
			transform.rotation.x, transform.rotation.y, transform.rotation.z,
			transform.scale.x, transform.scale.y, transform.scale.z,
			transformComponent.id
		);
	}

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
		gTransforms[0] = glm::mat4 (1.0f);
		TRANSFORM::ApplyModel (gTransforms[0], lTransforms[0].base);

		for (u64 i = 0; i < parenthoodsCount; ++i) {
			auto& componentParenthood = parenthoods[i];
			auto& parenthood = componentParenthood.base;
			auto& parent = componentParenthood.id;
			auto& parentGlobal = gTransforms[parent];

			spdlog::info ("pcc: {0}", parenthood.childrenCount);

			for (u64 j = 0; j < parenthood.childrenCount; ++j) {
				auto& child = parenthood.children[j];
				localSpace = parentGlobal;

				TRANSFORM::ApplyModel (localSpace, lTransforms[child].base);
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

		for (u16 iParenthood = 0; iParenthood < parenthoodsCount; ++iParenthood) {
		
			auto& componentParenthood = parenthoods[iParenthood];
			auto& parenthood = componentParenthood.base;
			auto& parentId = componentParenthood.id;
			auto& pLTransform = lTransforms[parentId];
		
			// Propagete DIRTY_FLAG further in the tree.
			if (pLTransform.flags & TRANSFORM::DIRTY) {
				for (u16 iChild = 0; iChild < parenthood.childrenCount; ++iChild) {
					auto& child = parenthood.children[iChild];
					auto& cTransform = lTransforms[child];
					cTransform.flags |= TRANSFORM::DIRTY;	// SET
				}
			}
		}
		
		for (u16 iParenthood = 0; iParenthood < parenthoodsCount; ++iParenthood) {
		
			auto& componentParenthood = parenthoods[iParenthood];
			auto& parenthood = componentParenthood.base;
			auto& parentId = componentParenthood.id;
			auto& pGTransform = gTransforms[parentId];
		
			// Apply and Clear DIRTY_FLAG in the tree.
			for (u16 iChild = 0; iChild < parenthood.childrenCount; ++iChild) {
		
				auto& childId = parenthood.children[iChild];
				auto& childLTransform = lTransforms[childId];
				auto& childGTransform = gTransforms[childId];
		
				if (childLTransform.flags & TRANSFORM::DIRTY) {
					childGTransform = pGTransform; // Each time copy from parent it's globalspace.
					TRANSFORM::ApplyModel (childGTransform, childLTransform.base);
					childLTransform.flags &= ~TRANSFORM::DIRTY; // CLEAR
				}
			}
		}
	}

	void ApplyDirtyFlagEx (
		const u64& parenthoodsCount, 
		PARENTHOOD::Parenthood* parenthoods,
		const u64& transformsCount, 
		TRANSFORM::LTransform* lTransforms,
		TRANSFORM::GTransform* gTransforms
	) {
		PROFILER { ZoneScopedN ("TRANSFROM:ApplyDirtyFlag"); }

		
		{ // ROOT
			auto& componentParenthood = parenthoods[0];
			auto& parenthood = componentParenthood.base;
			auto& parentId = componentParenthood.id;
			auto& pGTransform = gTransforms[parentId];
			auto& pLTransform = lTransforms[parentId];

			if (pLTransform.flags & TRANSFORM::DIRTY) {
				pGTransform = glm::mat4(1.0f);
				TRANSFORM::ApplyModel (pGTransform, pLTransform.base);
			}
		}
		
		for (u16 iParenthood = 0; iParenthood < parenthoodsCount; ++iParenthood) {
		
			auto& componentParenthood = parenthoods[iParenthood];
			auto& parenthood = componentParenthood.base;
			auto& parentId = componentParenthood.id;
			auto& pGTransform = gTransforms[parentId];
			auto& pLTransform = lTransforms[parentId];
			
			for (u16 iChild = 0; iChild < parenthood.childrenCount; ++iChild) {
		
				auto& childId = parenthood.children[iChild];
				auto& cLTransform = lTransforms[childId];
				auto& cGTransform = gTransforms[childId];

				// Propagete DIRTY_FLAG further in the tree.
				cLTransform.flags |= (pLTransform.flags & TRANSFORM::DIRTY);
		
				// Apply DIRTY_FLAG in the tree.
				if (cLTransform.flags & TRANSFORM::DIRTY) {
					cGTransform = pGTransform; // Each time copy from parent it's globalspace.
					TRANSFORM::ApplyModel (cGTransform, cLTransform.base);
				}
			}
		}
		
		// CLEAR all DIRTY_FLAGS ! The question is: Is this efficient?
		//  a single memset call maybe could optimize that further?
		for (u16 iTransform = 0; iTransform < transformsCount; ++iTransform) {
			auto& transform = lTransforms[iTransform];
			transform.flags &= ~TRANSFORM::DIRTY; // CLEAR
		}
	}

    void ApplyDirtyFlagSingle (
            TRANSFORM::LTransform& lTransform,
            TRANSFORM::GTransform& gTransform
    ) {
        PROFILER { ZoneScopedN ("TRANSFROM:ApplyDirtyFlagSingle"); }

        if (lTransform.flags & TRANSFORM::DIRTY) {
            gTransform = glm::mat4(1.0f);
            TRANSFORM::ApplyModel (gTransform, lTransform.base);
            lTransform.flags &= ~TRANSFORM::DIRTY;
        }
    }

}