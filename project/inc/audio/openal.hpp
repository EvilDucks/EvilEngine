#pragma once
#include "types.hpp"
#include "tool/debug.hpp"
#include "io.hpp"
#include "effolkronium/random.hpp"

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <AL/al.h>
#include <AL/alc.h>
#pragma GCC diagnostic pop

// According to the OpenAL documentation https://openal.org/documentation/OpenAL_Programmers_Guide.pdf
//  If source is attached to a buffer with STEREO sound, then this source is no longer affected by distance calculations.
//  Not sure how it affects our code, but it could mean that gain setting of a STEREO sound would stay unnafected??

// Changing gain and pitch.
//  Gain and pitch can be changed per sound, source and listener!
//  To allow gain above 1.0 the "AL_MAX_GAIN" must be set to the greater value!

// Sources
// https://www.openal.org/documentation/OpenAL_Programmers_Guide.pdf

// Issue! -> There can be only one listener.
//  About: Implemeneting multiplayer via split screen requires the use of more then one listener.
//  Solution: What is possible to do is to create a second and more devices with a new context with a new listener.
//  Ant thats the recommended way to do it.
// https://github.com/kcat/openal-soft/issues/711

#define ErrorSilentOpenAL(...) { \
	ErrorSilentMsg ("OPENAL", __VA_ARGS__); \
}


namespace AUDIO {

	using float3 = ALfloat[3];
	float3 ZERO { 0 }; 

	const float MAX_GAIN = 10.0f;

	void CheckError ( const char* message ) {
		const ALenum errorCode = alGetError ();
		switch (errorCode) {
			case AL_NO_ERROR: 																	return;
			case AL_INVALID_NAME:		ErrorSilentOpenAL ("AL_INVALID_NAME", message);			return;
			case AL_INVALID_ENUM:		ErrorSilentOpenAL ("AL_INVALID_ENUM", message);			return;
			case AL_INVALID_VALUE:		ErrorSilentOpenAL ("AL_INVALID_ENUM", message);			return;
			case AL_INVALID_OPERATION:	ErrorSilentOpenAL ("AL_INVALID_OPERATION", message);	return;
			case AL_OUT_OF_MEMORY:		ErrorSilentOpenAL ("AL_OUT_OF_MEMORY", message);		return;
			default:					ErrorSilentOpenAL ("UNKNOWN", message);					return;
		}
	}

}


namespace AUDIO::DEVICE {

	void Create (ALCdevice*& audioDevice) {
		const ALCchar* defaultDeviceString	= alcGetString (nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);	// nullptr means get default.
		audioDevice	= alcOpenDevice (defaultDeviceString);

		if (audioDevice == nullptr) ErrorExit ("OPENAL: Failed to get the default device.");		// release build check

		DEBUG_ENGINE LogInfo ("OpenAL Device: {0}", alcGetString (audioDevice, ALC_DEVICE_SPECIFIER));
	}

	void Destory (ALCdevice*& device) { alcCloseDevice (device); }

}


namespace AUDIO::CONTEXT {

	// OpenAL Context 
	//  is a ...

	auto Create (ALCdevice*& audioDevice, ALCcontext*& audioContext) {
		audioContext = alcCreateContext (audioDevice, /*attributelist*/ nullptr);
		if (!alcMakeContextCurrent (audioContext)) ErrorExit ("OPENAL: Failed to make the context active.");	// Activate the context and release check.
	}

	void Destory (ALCcontext*& audioContext) { 
		alcMakeContextCurrent (nullptr);				// Unreference context
		alcDestroyContext (audioContext);				// Free it's memory
	}

}


namespace AUDIO::LISTENER {

	void Create (
		/* IN  */ const float3& position,
		/* IN  */ const float3& velocity
	) {
		const ALfloat forwardAndUpVectors[] {
			/* forward */   1.0f, 0.0f, 0.0f,
			/* up */        0.0f, 1.0f, 0.0f
		};

		alListener3f (AL_POSITION, position[0], position[1], position[2]);
		DEBUG CheckError ("Listener: SET: POSITION");

		alListener3f (AL_VELOCITY, velocity[0], velocity[1], velocity[2]);
		DEBUG CheckError ("Listener: SET: VELOCITY");

		alListenerfv (AL_ORIENTATION, forwardAndUpVectors);
		DEBUG CheckError ("Listener: SET: ORIENTATION");
	}

}


namespace AUDIO::SOUND {

	void CreateMono ( 
		ALuint& sound,
		IO::WAV::Wav& mono
	) {
		alGenBuffers(1, &sound);
		DEBUG CheckError ("Sound: CREATE: BUFFORS");

		const auto depth = mono.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
		const ALsizei size = mono.pcmCount * sizeof (s16);
		alBufferData (sound, depth, mono.pcm, size, mono.sampleRate );
		DEBUG CheckError ("Sound: SET: BUFFORS");
	}

	void Destroy (ALuint& sound) {
		alDeleteBuffers (1, &sound);
		DEBUG CheckError ("Sound: DESTROY");
	}

}

namespace AUDIO::SOURCE {

	// ! MONO SOURCES ONLY WORK WITH MONO SOUNDS !

	void CreateMono (
		/* OUT */ ALuint& source,						// REF
		/* IN  */ const ALuint& sound, 					// REF
		/* IN  */ const float3& position,				// REF
		/* IN  */ const float3& velocity,				// REF
		/* IN  */ const ALboolean& islooped = false, 	// REF
		/* IN  */ const ALfloat& pitch = 1.0f, 			// REF
		/* IN  */ const ALfloat& gain = 1.0f			// REF
	) {
		alGenSources (1, &source);
		DEBUG CheckError ("Source: CREATE: BUFFORS");

		alSource3f (source, AL_POSITION, position[0], position[1], position[2]);
		DEBUG CheckError ("Source: SET: POSITION");

		alSource3f (source, AL_VELOCITY, velocity[0], velocity[1], velocity[2]);
		DEBUG CheckError ("Source: SET: VELOCITY");

		alSourcef (source, AL_MAX_GAIN, MAX_GAIN);
		DEBUG CheckError ("Source: SET: MAX_GAIN");

		alSourcef (source, AL_PITCH, pitch);
		DEBUG CheckError ("Source: SET: PITCH");

		alSourcef (source, AL_GAIN, gain);
		DEBUG CheckError ("Source: SET: GAIN");

		alSourcei(source, AL_LOOPING, islooped);
		DEBUG CheckError ("Source: SET: LOOP");

		alSourcei(source, AL_BUFFER, sound);
		DEBUG CheckError ("Source: SET: SOUND");
	}

	void CreateGlobalMono (
		/* OUT */ ALuint& source,						// REF
		/* IN  */ const ALuint& sound, 					// REF
		/* IN  */ const ALboolean& islooped = false, 	// REF
		/* IN  */ const ALfloat& pitch = 1.0f, 			// REF
		/* IN  */ const ALfloat& gain = 1.0f			// REF
	) { CreateMono (source, sound, ZERO, ZERO, islooped, pitch, gain); }

	void Destroy (ALuint& source) {
		alDeleteSources (1, &source);
		DEBUG CheckError ("Source: DESTROY");
	}

}


namespace AUDIO::STATE {

	void Play (const ALuint& source) {
        using Random = effolkronium::random_static;

		// -5.0f <-> +5.0f
		const s32 half = 5;
        const s32 random = Random::get (1, half * 2);
		const float pitch = 1 + (float)(random - half) / 100.f;

        alSourcef (source, AL_PITCH, pitch);
		alSourcePlay (source);
        DEBUG CheckError ("State: PLAY: Source could not start playing");
	}

	void Stop (const ALuint& source) {
		alSourceStop (source);
		DEBUG CheckError ("State: STOP: Source could not stop playing");
	}

}

namespace AUDIO {

	// Create a sound source for our stereo sound; note 3d positioning doesn't work with stereo files because
	//  stereo files are typically used for music. stereo files come out of both ears so it is hard to know
	//  what the sound should be doing based on 3d position data.
	//auto CreateStereoSource (
	//	const ALuint& sSoundBuffer, 
	//	const ALfloat& pitch = 1.0f, 
	//	const ALfloat& gain = 1.0f
	//) {
	//
	//	ALuint stereoSource;
	//	alGenSources(1, &stereoSource);
	//	DEBUG CheckError("8");
	//
	//	//alec(alSource3f(stereoSource, AL_POSITION, 0.f, 0.f, 1.f)); //NOTE: this does not work like mono sound positions!
	//	//alec(alSource3f(stereoSource, AL_VELOCITY, 0.f, 0.f, 0.f));
	//
	//	alSourcef(stereoSource, AL_PITCH, pitch);
	//	DEBUG CheckError("9");
	//	alSourcef(stereoSource, AL_GAIN, gain);
	//	DEBUG CheckError("10");
	//	alSourcei(stereoSource, AL_LOOPING, AL_FALSE);
	//	DEBUG CheckError("11");
	//
	//	alSourcei(stereoSource, AL_BUFFER, sSoundBuffer);
	//	DEBUG CheckError("12");
	//
	//	return stereoSource;
	//}

}