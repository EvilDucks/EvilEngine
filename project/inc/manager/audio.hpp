#pragma once
#include "tool/debug.hpp"
#include "render/gl.hpp"
#include "audio/openal.hpp"

namespace MANAGER::AUDIO {

	::AUDIO::IO::WAV::Wav loader {};

	enum SOURCES: u16 {
		SPRING_TRAP_ACTIVATE 	= 0,
		FALL_IMPACT 			= 1,
		JUMP 					= 2,
		CHECKPOINT_NEW 			= 3,
		CHECKPOINT_TAKING 		= 4,
		VICTORY 				= 5,
		POWER_UP 				= 6,
		JUMP_DOUBLE 			= 7,
		DUNNO 					= 8,
	};

	// mono!
	const u8 SOUNDS_COUNT = 10;
	ALuint sounds[SOUNDS_COUNT];
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

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_CHECKPOINT_NEW, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[4], loader);
		::AUDIO::IO::WAV::Destory 	(loader);

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_CHECKPOINT_BEAM, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[5], loader);
		::AUDIO::IO::WAV::Destory 	(loader);

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_VICTORY, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[6], loader);
		::AUDIO::IO::WAV::Destory 	(loader);

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_POWER_UP, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[7], loader);
		::AUDIO::IO::WAV::Destory 	(loader);

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_DOUBLE_JUMP, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[8], loader);
		::AUDIO::IO::WAV::Destory 	(loader);

		::AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_CHECKPOINT_BEAM, loader);
		::AUDIO::SOUND::CreateMono 	(sounds[9], loader);
		::AUDIO::IO::WAV::Destory 	(loader);
	}

	void DestroySounds () {
		for (u8 i = 0; i < SOUNDS_COUNT; ++i) {
			::AUDIO::SOUND::Destroy (sounds[i]);
		}
	}

	void CreateGlobalSources (const float& gain = 1.0f) {
		// SOUND
		auto& musicSound = sounds[0];
		// SOURCE
		::AUDIO::SOURCE::CreateGlobalMono	(musicSource, musicSound, true, 1.0f, gain);
		// STATE
		::AUDIO::STATE::Play 				(musicSource);
	}

	void CreateGlobalSource (const ALuint& sound, const ::AUDIO::float3& position, const float& gain = 1.0f) {
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
