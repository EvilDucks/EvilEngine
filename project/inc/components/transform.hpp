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
		model = glm::translate (model, local.position);
		model = glm::scale (model, local.scale);
		model = glm::rotate (model, glm::radians (local.rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
		model = glm::rotate (model, glm::radians (local.rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
		model = glm::rotate (model, glm::radians (local.rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
	}

}