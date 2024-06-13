#pragma once
#include "tool/debug.hpp"
#include "render/gl.hpp"
#include "audio/openal.hpp"

namespace GLOBAL { // mono!

	ALuint sources[64];
	u8 sourcesCounter = 0;

	void PlaySource (const u8& id) {
		AUDIO::STATE::Play 				(sources[id]);
	}
	
}

namespace AUDIOSOURCE {

	struct Base {

	};

	struct AudioSource {
		Base base;
	};

}
