#pragma once
#include "tool/debug.hpp"

namespace OBJECT {

	// ! as of right now theres one extra transfrom thats not used ! ENTITY_5
	// These are for now only...
	const u16 _1 = 1;
	const u16 _2 = 2;
	const u16 _3 = 3;
	const u16 _4 = 4;
	const u16 _5 = 5;
	const u16 _6 = 6;
    const u16 _player = 7;

	const GameObjectID ID_INVALID = -1;
	const GameObjectID ID_DEFAULT = 0;

	// Sure // Guess

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
		/* OUT */ u64& startIndex,
		/* IN  */ const u64& arrayCount, 
		/* IN  */ const A* const arrayPointer, 
		/* IN  */ const GameObjectID& id
	) {
		for (; arrayPointer[startIndex].id != id; ++startIndex);
	}

}