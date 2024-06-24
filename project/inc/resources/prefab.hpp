#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "manager.hpp"
#include "json.hpp"
#include "util/mmrelation.hpp"

namespace RESOURCES::PREFAB {

    const char* NODE = "prefab";

    const char* PREFAB_RESERVED    = "\0";
    const char* PREFAB_PLAYER     = "PLAYER";
    const char* PREFAB_INVALID    = "INVALID";

    const u8 PREFABS_COUNT = 3;

    const char* NAME_PREFABS[PREFABS_COUNT] {
        PREFAB_RESERVED,    // 0 is reserved - it means not a prefab!
        PREFAB_PLAYER,
        PREFAB_INVALID,     // last is for error checking !
    };

    enum IDS: u8 {
        PLAYER      = 0,
        INVALID     = 1,
    };


    // EXAMPLE
	// mmrlut [ !, n, !, n, !, n, !] // where ! -> NOT_REPRESENTIVE, n -> valid mmrelation
	// plut   [ 0, 0, 0, 0, 1, 0, 2] // where 0 -> non a prefab
	//	=>
	// mmrlut [ !, !, !, !, n, n, n]
	// plut   [ 0, 0, 1, 2, 0, 0, 0]
	//
	//  Go through initial mmrlut and add each NOT_REPRESENTIVE position to a new array
	//  
    void SortPrefabTable (
        /* OUT */ const u16& mmrlutc,
        /* OUT */ u16*& mmrlut,
        /* OUT */ u8*& plut
    ) {
		u16 plutc = 0;
		for (u16 iRelation = 0; iRelation < mmrlutc; ++iRelation) {
			auto& relation = mmrlut[iRelation];
			if (relation == MMRELATION::NOT_REPRESENTIVE) {
				plut[plutc] = plut[iRelation]; // it should never override.. i suppose
				++plutc;
			}
		}

		// Reset the ones in front to 0.
		memset (plut + plutc, 0, (mmrlutc - plutc) * sizeof (u8));
	}

}
