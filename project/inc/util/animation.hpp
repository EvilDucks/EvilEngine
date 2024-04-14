#pragma once
#include "tool/debug.hpp"

namespace ANIMATION {

	struct Animation {
		const float frameTime;
		const u8 frameMax;
		const u8 frameMin;
		float passedTime;
		u8 frameCurrent;
	};

	void Update (Animation& anim, const float& timeDelta) {
		anim.passedTime += timeDelta;
		u8 timeCondition = anim.passedTime >= anim.frameTime;
		anim.passedTime *= (!timeCondition);
		anim.frameCurrent += (timeCondition);
		u8 frameCondition = anim.frameCurrent >= anim.frameMax;
		anim.frameCurrent = (anim.frameCurrent * !frameCondition) + (anim.frameMin * frameCondition);
	}

}