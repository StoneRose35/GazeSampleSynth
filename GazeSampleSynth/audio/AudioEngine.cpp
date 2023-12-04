#include "AudioEngine.h"
#include "Windows.h"
#include "Mmdeviceapi.h"
#include <Functiondiscoverykeys_devpkey.h>
#include "Audioclient.h"
#include <cstdio>

#define REFTIMES_PER_SEC   100000 // 10000000 
#define REFTIMES_PER_MILLISEC  100


DWORD WINAPI AudioThread(LPVOID lpParam);

bool AudioEngine::initAudioEngine()
{
    IMMDeviceEnumerator* pEnumerator;
    IMMDeviceCollection* pDevices;
    IMMDevice* audioDevice;
    UINT nDevices;
    IAudioClient* pAudioClient = NULL;
    WAVEFORMATEX* pwfx = NULL;
    IAudioRenderClient* pRenderClient = NULL;
    IPropertyStore* pPropertyStore = NULL;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    BYTE* pData;
    DWORD flags = 0;
    DWORD threadId;
    HRESULT hr;
    PROPVARIANT name;
    const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
    const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
    const IID IID_IAudioClient = __uuidof(IAudioClient);
    const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
    hr = CoInitialize(NULL);
    hr = CoCreateInstance(
        CLSID_MMDeviceEnumerator, NULL,
        CLSCTX_ALL, IID_IMMDeviceEnumerator,
        (void**)&pEnumerator);
    hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
    hr = pDevices->GetCount(&nDevices);
    printf("got %d devices\n", nDevices);
    hr = pDevices->Item(0, &audioDevice);


    hr = pEnumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, &audioDevice);

    hr = audioDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
    PropVariantInit(&name);
    hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &name);
    printf("Friendly Name of Audio Device: %S", name.pwszVal);
    printf("\n");

    hr = audioDevice->Activate(IID_IAudioClient, CLSCTX_ALL,
        NULL, (void**)&pAudioClient);

    hr = pAudioClient->GetMixFormat(&pwfx);
    if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        WAVEFORMATEXTENSIBLE* wex;
        wex = (WAVEFORMATEXTENSIBLE*)pwfx;
        GUID sf = wex->SubFormat;
        if (sf != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
        {
            return false;
        }
    }
    else if (pwfx->wFormatTag != WAVE_FORMAT_IEEE_FLOAT)
    {
        return false;
    }


    hr = pAudioClient->Initialize(
            AUDCLNT_SHAREMODE_SHARED,
            0,
            hnsRequestedDuration,
            0,
            pwfx,
            NULL);
    
    // at this point the application can be informed about the bit depth, format 
    // and sampling rate
        


    // Get the actual size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);


    hr = pAudioClient->GetService(
        IID_IAudioRenderClient,
        (void**)&pRenderClient);


    // Grab the entire buffer for the initial fill operation.
    hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);


    // Load the initial data into the shared buffer.
    //hr = pMySource->LoadData(bufferFrameCount, pData, &flags);
    // zero the entire buffer
    for (UINT c = 0; c < ((pwfx->wBitsPerSample * pwfx->nChannels) >> 3)*bufferFrameCount; c++)
    {
        *(pData + c) = 0;
    }


    hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);


    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
    bufferFrameCount / pwfx->nSamplesPerSec;
    audioData.actualDuration = hnsActualDuration;
    audioData.audioClient = pAudioClient;
    audioData.pData = pData;
    audioData.bufferFrameCount = bufferFrameCount;
    audioData.flags = flags;
    audioData.renderClient = pRenderClient;
    for (uint8_t c = 0; c < N_SOUNDGENERATORS; c++)
    {
        audioData.soundGenerators[c] = NULL;
    }

    thread = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        AudioThread,       // thread function name
        &audioData,          // argument to thread function 
        0,                      // use default creation flags 
        &threadId);   // returns the thread identifier 

    return true;
}

bool AudioEngine::stopAudioEngine()
{
    audioData.flags = AUDCLNT_BUFFERFLAGS_SILENT;
    return true;
}

int AudioEngine::addSoundGenerator(MusicalSoundGenerator* generator)
{
    int res = -1;
    for (uint8_t c = 0; c < N_SOUNDGENERATORS; c++)
    {
        if (audioData.soundGenerators[c] == NULL && res == -1)
        {
            audioData.soundGenerators[c] = generator;
            res = c;
        }
    }
    return res;
}

MusicalSoundGenerator* AudioEngine::getSoundGenerator(int idx)
{
    return audioData.soundGenerators[idx];
}

DWORD WINAPI AudioThread(LPVOID lpParam)
{
    PAUDIODATA audioData = (PAUDIODATA)lpParam;
    UINT32 numFramesPadding, numFramesAvailable;
    float audioSum = 0.0f, avgAudioSum=0.0f;
    float* floatData;
    BYTE* dataPtr;
    audioData->audioClient->Start();
    while (audioData->flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {

        // Sleep for half the buffer duration.
        //Sleep((DWORD)(audioData->actualDuration / REFTIMES_PER_MILLISEC / 2));

        // See how much buffer space is available.
       audioData->audioClient->GetCurrentPadding(&numFramesPadding);

        numFramesAvailable = audioData->bufferFrameCount - numFramesPadding;

        // Grab all the available space in the shared buffer.
        audioData->renderClient->GetBuffer(numFramesAvailable, &dataPtr);

        floatData = (float*)dataPtr;
        // Get next 1/2-second of data from the audio source.
        // The show happens here!!!
        //hr = pMySource->LoadData(numFramesAvailable, pData, &flags);
        avgAudioSum = 0.0f;
        for (UINT q = 0; q < numFramesAvailable; q++)
        {
            audioSum = 0.0f;
            for (uint8_t c = 0; c < N_SOUNDGENERATORS; c++)
            {
                if (audioData->soundGenerators[c] != NULL)
                {
                    audioSum += audioData->soundGenerators[c]->getNextSample();
                }
            }
            //audioSum *= 0.0f;
            avgAudioSum += audioSum * audioSum;
            *(floatData + q * 2) = audioSum;
            *(floatData + q * 2 + 1) = audioSum;
        }

        audioData->renderClient->ReleaseBuffer(numFramesAvailable, audioData->flags);

    }
    audioData->audioClient->Stop();  // Stop playing.
    return 0;
}