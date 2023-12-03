#pragma once

#include "Windows.h"
#include "Mmdeviceapi.h"
#include "Audioclient.h"

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

