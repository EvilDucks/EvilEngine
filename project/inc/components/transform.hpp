#pragma once
#include "tool/debug.hpp"
#include "render/gl.hpp"

namespace TRANSFORM {

	//namespace DDD {
	//}

	//namespace DD {
	//}

	//struct Position {
	//    r32 x, y, z;
	//};
	//struct Rotation {
	//    r32 x, y, z;
	//};
	//struct Scale {
	//    r32 x, y, z;
	//};

	using Position = glm::vec3;
	using Rotation = glm::vec3;
	using Scale = glm::vec3;

	struct Base {
		Position position;
		Rotation rotation;
		Scale scale;
	};

	struct Transform {
		GameObjectID id = 0;				//8b
		glm::mat4 global = glm::mat4(1.0f); //64b
		Base local;							//9*4b
	};


	void ApplyModel (glm::mat4& model, const Base& local) {
		model = glm::translate (model, local.position);
		model = glm::scale (model, local.scale);
		model = glm::rotate (model, glm::radians (local.rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
		model = glm::rotate (model, glm::radians (local.rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
		model = glm::rotate (model, glm::radians (local.rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
	}

}