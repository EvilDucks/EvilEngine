#pragma once
#include "tool/debug.hpp"

namespace OBJECT {

	const GameObjectID ID_DEFAULT = 0;

	template<class A>
	void GetComponentSlow (
		/* OUT */ u64& elementIndex,
		/* IN  */ const u64& arrayCount, 
		/* IN  */ const A* const arrayPointer, 
		/* IN  */ const GameObjectID& id
	) {
		for (u64 i = 0; i < arrayCount; ++i) {
			if (arrayPointer[i].id == id) {
				elementIndex = i;
				break;
			}
		}
	}

	template<class A>
	void GetComponentFast (
		/* OUT */ u64& elementIndex,
		/* IN  */ const u64& arrayCount, 
		/* IN  */ const A* const arrayPointer, 
		/* IN  */ const GameObjectID& id
	) {
		for (; arrayPointer[elementIndex].id != id; ++elementIndex);
	}

}