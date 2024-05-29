#pragma once
#include "tool/debug.hpp"
#include "render/gl.hpp"

namespace RECTANGLE {

	using Position		= glm::vec2;
	using Size			= glm::vec2;
	using Anchor		= glm::vec2;
	using Pivot			= glm::vec2;
	using Rotation		= r32;
	using Scale			= glm::vec2;

	using GRectangle	= glm::mat4; // 16 x 4b as it is r32

	const u8 NOT_DIRTY = 0;
	const u8 DIRTY = 1;

	struct Base {
		Anchor anchor;		// Tell to where in screen we're snapped to.
		Position position;	// Offset from anchor position.
		Size size;			// Width and Height of a control.
		Pivot pivot;		// Rotation point.
		Rotation rotation;	// Single axis rotation.
		Scale scale;		// Additional option to scale the size. (for example multi resolution support).
		// 
		u8 flags;			// 1b
	};

	struct LRectangle {
		GameObjectID id = 0;				// 2b
		Base base;							// 9 * 4b
	};


	void ApplyModel (glm::mat4& model, const Base& base, const r32& viewportX, const r32& viewportY) {
		const glm::vec2 aPosition (
			(viewportX * base.anchor.x) + base.position.x,
			(viewportY * base.anchor.y) + base.position.y
		);

		model = glm::translate	(model, glm::vec3 (aPosition.x, aPosition.y, 0.0f));

		model = glm::translate	(model, glm::vec3 (base.pivot.x, base.pivot.y, 0.0f));
		model = glm::rotate		(model, glm::radians (base.rotation), glm::vec3 (0.0f, 0.0f, 1.0f));
		model = glm::translate	(model, glm::vec3 (-base.pivot.x, -base.pivot.y, 0.0f));

		model = glm::scale		(model, glm::vec3 (base.size.x, base.size.y, 1.0f));
		model = glm::scale		(model, glm::vec3 (base.scale.x, base.scale.y, 1.0f));
	}

}