#pragma once
#include "tool/debug.hpp"
#include "render/gl.hpp"

namespace RECTANGLE {

    using Position		= glm::vec2;
    using Anchor		= glm::vec2;
	using Rotation		= glm::vec3;
	using Scale			= glm::vec2;

	using GTransform	= glm::mat4; // 16 x 4b as it is r32

	const u8 NOT_DIRTY = 0;
	const u8 DIRTY = 1;

	struct Base {
		Position position;
        Anchor anchor;
		Rotation rotation;
		Scale scale;
        // 
        u8 flags;							// 1b
	};

	struct LTransform {
		GameObjectID id = 0;				// 2b
		Base base;							// 9 * 4b
	};


	void ApplyModel (glm::mat4& model, const Base& base) {
		model = glm::translate	(model, glm::vec3 (base.position.x, base.position.y, 1.0f));
		model = glm::rotate		(model, glm::radians (base.rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
		model = glm::rotate		(model, glm::radians (base.rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
		model = glm::rotate		(model, glm::radians (base.rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
		model = glm::scale		(model, glm::vec3 (base.scale.x, base.scale.y, 1.0f));
	}

}