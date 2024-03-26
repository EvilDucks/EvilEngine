#pragma once
#include "tool/debug.hpp"
#include "render/gl.hpp"

namespace TRANSFORM {

	// Transfrom could be implemented in 2 different ways.

	// 1. Precalculating values
	//  a. We could during rendering phase simply attach a correct precalculated globalspace Transfrom.
	//   - This however would mean that 1. We need to calculate that during Initialization & 2. If we 
	//    change that value during execution we need to refer to parent/children objects and on top of 
	//    that store their localspace transfroms as globalspace t. wouldn't be enough.

	//1 using Base = glm::mat4;
	//1 struct Transform {
	//1 	GameObjectID id = 0;
	//1 	Base global = glm::mat4(1.0f); 
	//1 	Base local = glm::mat4(1.0f);
	//1 };
	
	// 2. Calculating values on fly.
	//  a. Instead of refering to precalculated globalspace we calculate it on the fly
	//   - to do so all we need is a parent transfrom id. So we have 'u64 (4b)' vs 'glm::mat4 (64b)'
	//  b. Also we don't need to store a glm::mat4 'vec3<r32>[3] (36b)' is smaller then 'glm::mat4 (64b)'
	//  c. Simply if parent_id = 0 we're at root.

	//2 NEVERMIND

	using Position = glm::vec3;
	using Rotation = glm::vec3;
	using Scale = glm::vec3;
	
	struct Base {
	    Position position = glm::vec3(0.0f);
	    Rotation rotation = glm::vec3(0.0f);
	    Scale scale = glm::vec3(1.0f);
	};

	struct Transform {
		GameObjectID id = 0;
		Base global;
		Base local;
	};

	void ApplyTransform (glm::mat4& model, const Base& transfrom) {
		model = glm::translate (model, transfrom.position);
		model = glm::scale (model, transfrom.scale);
		model = glm::rotate (model, glm::radians (transfrom.rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
		model = glm::rotate (model, glm::radians (transfrom.rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
		model = glm::rotate (model, glm::radians (transfrom.rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
	}

	void GetUnpacked (
		/* IN  */ const glm::mat4& transform,
		/* OUT */ glm::vec3& translation,
		/* OUT */ glm::quat& rotation,
		/* OUT */ glm::vec3& scale,
		/* OUT */ glm::vec4& perspective,
		/* OUT */ glm::vec3& skew
	) {
		glm::decompose (transform, scale, rotation, translation, skew, perspective);
		rotation = glm::conjugate(rotation);
	}

}


// OLD
//	struct Position {
//	    r32 x, y, z;
//	};	
//	struct Rotation {
//	    r32 x, y, z;
//	};
//	struct Scale {
//	    r32 x, y, z;
//	};
