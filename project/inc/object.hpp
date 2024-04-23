#pragma once
#include "tool/debug.hpp"

namespace OBJECT {

	// ! as of right now theres one extra transfrom thats not used ! ENTITY_5
	// These are for now only...
	const u16 _01 					= 1;
	const u16 _02 					= 2;
	const u16 _03 					= 3; // root
	const u16 _04 					= 4;
	const u16 _05 					= 5;
	const u16 _06 					= 6;
    const u16 _07_player 			= 7;
    const u16 _08_testWall 			= 8;
	const u16 _09_SQUARE_1 			= 9;
	const u16 _10_SQUARE_2 			= 10;
	const u16 _11_SKYBOX 			= 11;
	const u16 _12_GROUND 			= 12;
	const u16 _13_LIGHT_1 			= 13;

	const GameObjectID ID_INVALID	= -1;
	const GameObjectID ID_DEFAULT	= 0;

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