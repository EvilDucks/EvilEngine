#pragma once
#include "tool/debug.hpp"
#include "render/gl.hpp"

namespace ROTATING {

    using Base = glm::vec3;

    struct Rotating {
		GameObjectID id = 0;
		Base base;
	};

}