#pragma once
#include "tool/debug.hpp"
#include "render/gl.hpp"
#include "audio/openal.hpp"

namespace MANAGER::AUDIO {

	::AUDIO::IO::WAV::Wav loader {};

	// mono!
	ALuint sounds[8];
	ALuint musicSource;
	ALuint sources[64];
	u8 sourcesCounter = 0;

	void CreateSounds () {
		auto& musicSound = sounds[0];

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_TEST, loader);
		::AUDIO::SOUND::CreateMono 	(musicSound, loader);
		::AUDIO::IO::WAV::Destory 	(loader);

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_SPRINT_TRAP_ACTIVATE, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[1], loader);
		::AUDIO::IO::WAV::Destory 	(loader);

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_FALL_IMPACT, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[2], loader);
		::AUDIO::IO::WAV::Destory 	(loader);

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_JUMP, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[3], loader);
		::AUDIO::IO::WAV::Destory 	(loader);

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_NEW_CHECKPOINT, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[4], loader);
		::AUDIO::IO::WAV::Destory 	(loader);

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_VICTORY, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[5], loader);
		::AUDIO::IO::WAV::Destory 	(loader);

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_VICTORY, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[6], loader);
		::AUDIO::IO::WAV::Destory 	(loader);

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_POWER_UP, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[7], loader);
		::AUDIO::IO::WAV::Destory 	(loader);
	}

	void DestroySounds () {
		auto& musicSound = sounds[0];

		::AUDIO::SOUND::Destroy (musicSound);
		::AUDIO::SOUND::Destroy (sounds[1]);
		::AUDIO::SOUND::Destroy (sounds[2]);
		::AUDIO::SOUND::Destroy (sounds[3]);
		::AUDIO::SOUND::Destroy (sounds[4]);
		::AUDIO::SOUND::Destroy (sounds[5]);
		::AUDIO::SOUND::Destroy (sounds[6]);
		::AUDIO::SOUND::Destroy (sounds[7]);
	}

	void CreateGlobalSources (const float& gain = 1.0f) {
		// SOUND
		auto& musicSound = sounds[0];
		// SOURCE
		::AUDIO::SOURCE::CreateGlobalMono	(musicSource, musicSound, true, 1.0f, gain);
		// STATE
		::AUDIO::STATE::Play 				(musicSource);
	}

	void CreateSource (const ALuint& sound, const ::AUDIO::float3& position, const float& gain = 1.0f) {
		auto& source = sources[sourcesCounter];
		++sourcesCounter;

		::AUDIO::SOURCE::CreateMono 		(source, sound, position, ::AUDIO::ZERO, false, 1.0f, gain);
	}

	void DestroySources () {
		::AUDIO::STATE::Stop 				(musicSource);
		::AUDIO::SOURCE::Destroy 			(musicSource);

		for (u8 i = 0; i < sourcesCounter; ++i) {
			auto& source = sources[i];
			::AUDIO::STATE::Stop 			(source);
			::AUDIO::SOURCE::Destroy		(source);
		}
	}

	void PlaySource (const u8& id) {
		::AUDIO::STATE::Play 				(sources[id]);
	}

	void StopSource (const u8& id) {
		::AUDIO::STATE::Stop 				(sources[id]);
	}

}
