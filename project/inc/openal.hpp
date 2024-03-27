#pragma once
#include "types.hpp"

#include <al.h>
#include <alc.h>

// According to the OpenAL documentation https://openal.org/documentation/OpenAL_Programmers_Guide.pdf
//  If source is attached to a buffer with STEREO sound, then this source is no longer affected by distance calculations.
//  Not sure how it affects our code, but it could mean that gain setting of a STEREO sound would stay unnafected??


// Changing gain and pitch.
//  Gain and pitch can be changed per sound, source and listener!
//  To allow gain above 1.0 the "AL_MAX_GAIN" must be set to the greater value!


namespace OpenAL {

    const float MAX_GAIN = 10.0f;


    auto CheckError(
            const char* message
    ) {
        const ALenum errorCode = alGetError();
        switch (errorCode) {
            case AL_NO_ERROR:
                return;
            case AL_INVALID_NAME:
                //std::cerr << OPENAL_ERROR << "AL_INVALID_NAME" << " with: " << message << std::endl;
                return;
            case AL_INVALID_ENUM:
                //std::cerr << OPENAL_ERROR << "AL_INVALID_ENUM" << " with: " << message << std::endl;
                return;
            case AL_INVALID_VALUE:
                //std::cerr << OPENAL_ERROR << "AL_INVALID_VALUE" << " with: " << message << std::endl;
                return;
            case AL_INVALID_OPERATION:
                //std::cerr << OPENAL_ERROR << "AL_INVALID_OPERATION" << " with: " << message << std::endl;
                return;
            case AL_OUT_OF_MEMORY:
                //std::cerr << OPENAL_ERROR << "AL_OUT_OF_MEMORY" << " with: " << message << std::endl;
                return;
            default:
                //std::cerr << OPENAL_ERROR << "UNKNOWN: " << errorCode << " with: " << message << std::endl;
                return;
        }
    }


    auto DestroySource(ALuint& source) {
        alDeleteSources(1, &source);
        OpenAL::CheckError("destory-source");
    }


    auto DestorySound(ALuint& soundBuffer) {
        alDeleteBuffers(1, &soundBuffer);
        OpenAL::CheckError("destory-sound");
    }


    auto DestoryContext(ALCcontext* context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
    }


    auto DestoryDevice(ALCdevice* device) {
        alcCloseDevice(device);
    }


    auto CreateAudioDevice() {

        // First arg - device, nullptr means get default.
        const ALCchar* defaultDeviceString = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
        ALCdevice* device = alcOpenDevice(defaultDeviceString);

        if (device == nullptr) {
            //std::cerr << OPENAL_FAIL_GET_DEFAULT_DEVICE << std::endl;
            exit(1);
        }

        return device;
    }


    auto CreateAudioContext(ALCdevice* device) {

        // Create context for that device. ( It's a state machine with its own sound sources. )
        ALCcontext* context = alcCreateContext(device, /*attributelist*/ nullptr);

        // Activates the context!
        if (!alcMakeContextCurrent(context)) {
            //std::cerr << "Failed to make the context active." << std::endl;
            exit(1);
        }

        return context;
    }


    // Creates a listener in 3D space. ( User that hears the sound. )
    auto CreateListener3D() {

        const ALfloat forwardAndUpVectors[] {
                /* forward */   1.0f, 0.0f, 0.0f,
                /* up */        0.0f, 1.0f, 0.0f
        };

        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
        OpenAL::CheckError("listener-position");
        alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        OpenAL::CheckError("listener-velocity");
        alListenerfv(AL_ORIENTATION, forwardAndUpVectors);
        OpenAL::CheckError("listener-orientation");

    }


    // Create a sound source that plays our mono sound. ( From the sound buffer. )
    auto CreateMonoSource(const ALuint& mSoundBuffer, const ALboolean& islooped = false, const ALfloat& pitch = 1.0f, const ALfloat& gain = 1.0f) {

        ALuint monoSource;
        alGenSources(1, &monoSource);
        OpenAL::CheckError("1");


        alSource3f(monoSource, AL_POSITION, 1.f, 0.f, 0.f);
        OpenAL::CheckError("2");
        alSource3f(monoSource, AL_VELOCITY, 0.f, 0.f, 0.f);
        OpenAL::CheckError("3");

        // Set max gain for that sound. // Requires source.
        alSourcef(monoSource, AL_MAX_GAIN, MAX_GAIN);
        OpenAL::CheckError("MaxGain");
        alSourcef(monoSource, AL_PITCH, pitch);
        OpenAL::CheckError("4");
        alSourcef(monoSource, AL_GAIN, gain);
        OpenAL::CheckError("5");
        alSourcei(monoSource, AL_LOOPING, islooped);
        OpenAL::CheckError("6");

        alSourcei(monoSource, AL_BUFFER, mSoundBuffer);
        OpenAL::CheckError("7");

        return monoSource;
    }

    // Create a sound source for our stereo sound; note 3d positioning doesn't work with stereo files because
    //  stereo files are typically used for music. stereo files come out of both ears so it is hard to know
    //  what the sound should be doing based on 3d position data.
    auto CreateStereoSource(const ALuint& sSoundBuffer, const ALfloat& pitch = 1.0f, const ALfloat& gain = 1.0f) {

        ALuint stereoSource;
        alGenSources(1, &stereoSource);
        OpenAL::CheckError("8");

        //alec(alSource3f(stereoSource, AL_POSITION, 0.f, 0.f, 1.f)); //NOTE: this does not work like mono sound positions!
        //alec(alSource3f(stereoSource, AL_VELOCITY, 0.f, 0.f, 0.f));

        alSourcef(stereoSource, AL_PITCH, pitch);
        OpenAL::CheckError("9");
        alSourcef(stereoSource, AL_GAIN, gain);
        OpenAL::CheckError("10");
        alSourcei(stereoSource, AL_LOOPING, AL_FALSE);
        OpenAL::CheckError("11");

        alSourcei(stereoSource, AL_BUFFER, sSoundBuffer);
        OpenAL::CheckError("12");

        return stereoSource;
    }

    auto StopSound(const ALuint& source) {
        alSourceStop(source);
    }



}