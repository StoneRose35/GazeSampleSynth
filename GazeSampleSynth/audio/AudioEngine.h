#pragma once

#include "Windows.h"
#include "Mmdeviceapi.h"
#include "Audioclient.h"
#include "MusicalSoundGenerator.h"

#define MIDI_AVAILABLE_MSK 0x200
#define MIDI_NOTE_CHANGE_MSK 0x100
#define MIDI_TAKEN_MSK 0x80
#define N_SOUNDGENERATORS 24

typedef struct {
    DWORD actualDuration;
    DWORD flags;
    IAudioClient* audioClient;
    IAudioRenderClient* renderClient;
    UINT32 bufferFrameCount;
    BYTE* pData;
    MusicalSoundGenerator* soundGenerators[N_SOUNDGENERATORS];
} AUDIODATA, * PAUDIODATA;

class AudioEngine
{
public:
	bool initAudioEngine();
    bool stopAudioEngine();
    int addSoundGenerator(MusicalSoundGenerator* generator);
    MusicalSoundGenerator* getSoundGenerator(int idx);
private:
	HANDLE thread;
	AUDIODATA audioData;
};

