#pragma once
#include "tool/debug.hpp"

namespace PARENTHOOD {

	struct Base {
		u16 childrenCount = 0;
		GameObjectID* children = nullptr;
	};

	struct Parenthood {
		GameObjectID id = 0;
		Base base { 0 };
	};

}