#pragma once
#include "types.hpp"
#include "tool/debug.hpp"

#include "dr_wav.h"

namespace AUDIO::IO::WAV {

	struct Wav {
		drwav_uint64 totalPCMFrameCount;
		u32 sampleRate;
		u32 channels;

		u64 pcmCount;
		s16* pcm;
	};

	drwav_uint64 GetTotalSamples (const Wav& wav) {
		return wav.totalPCMFrameCount * wav.channels;
	}

	void Load (const char* const filepath, Wav& mono) {

		drwav_int16* data = drwav_open_file_and_read_pcm_frames_s16 (
			filepath, &mono.channels, &mono.sampleRate, 
			&mono.totalPCMFrameCount, nullptr
		);
	
		if (data == nullptr) {
			drwav_free (data, nullptr);
			ErrorExit ("{0}: Could not load the file. Check it's filepath.", "AUDIO->IO");
		}

		mono.pcmCount = (u64) GetTotalSamples (mono);
	
		if (GetTotalSamples (mono) > (drwav_uint64)(std::numeric_limits<size_t>::max())) {
			drwav_free (data, nullptr);
			ErrorExit ("{0}: File is to large for 32bit addressing! Use a smaller file!", "AUDIO->IO");
		}
		
		mono.pcm = new s16[mono.pcmCount];							// Allocation
		memcpy (mono.pcm, data, mono.pcmCount * sizeof (s16));		// CPY
		drwav_free (data, nullptr);									// FREE
	}


	//void LoadStereo (const char* const filepath, Wav& stereo) {
	//
	//	drwav_int16* data = drwav_open_file_and_read_pcm_frames_s16 (
	//		filepath, &stereo.channels, &stereo.sampleRate, 
	//		&stereo.totalPCMFrameCount, nullptr
	//	);
	//
	//	if (data == nullptr) {
	//		drwav_free (data, nullptr);
	//		ErrorExit ("{0}: Could not load the file. Check it's filepath.", "AUDIO->IO");
	//	}
	//
	//	stereo.pcmCount = (u64) GetTotalSamples (stereo);
	//
	//	if (GetTotalSamples (stereo) > (drwav_uint64)(std::numeric_limits<size_t>::max())) {
	//		drwav_free (data, nullptr);
	//		ErrorExit ("{0}: File is to large for 32bit addressing! Use a smaller file!", "AUDIO->IO");
	//	}
	//
	//	stereo.pcm = new s16[stereo.pcmCount];
	//	memcpy (stereo.pcm, data, stereo.pcmCount * sizeof (s16));
	//	drwav_free (data, nullptr);
	//
	//}


	void Destory (Wav& sound) { delete[] sound.pcm; }

}