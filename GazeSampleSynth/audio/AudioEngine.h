#pragma once

#include "Windows.h"
#include "Mmdeviceapi.h"
#include "Audioclient.h"

#define MIDI_AVAILABLE_MSK 0x200
#define MIDI_NOTE_CHANGE_MSK 0x100
#define MIDI_TAKEN_MSK 0x80

typedef struct {
    DWORD actualDuration;
    DWORD flags;
    IAudioClient* audioClient;
    IAudioRenderClient* renderClient;
    UINT32 bufferFrameCount;
    BYTE* pData;
} AUDIODATA, * PAUDIODATA;

class AudioEngine
{
public:
	bool initAudioEngine();
    bool stopAudioEngine();
private:
	HANDLE thread;
	AUDIODATA audioData;
};

