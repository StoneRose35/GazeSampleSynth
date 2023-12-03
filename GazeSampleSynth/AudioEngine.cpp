#include "AudioEngine.h"
#include "Windows.h"
#include "Mmdeviceapi.h"
#include "Audioclient.h"

#define REFTIMES_PER_SEC  100000
#define REFTIMES_PER_MILLISEC  100

DWORD WINAPI AudioThread(LPVOID lpParam);


bool AudioEngine::initAudioEngine()
{
    IMMDeviceEnumerator* pEnumerator;
    IMMDeviceCollection* pDevices;
    IMMDevice* audioDevice;
    wchar_t strBfr[256];
    UINT nDevices;
    IAudioClient* pAudioClient = NULL;
    WAVEFORMATEX* pwfx = NULL;
    IAudioRenderClient* pRenderClient = NULL;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    BYTE* pData;
    DWORD flags = 0;
    DWORD threadId;
    HRESULT hr;
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
    wsprintf(strBfr, L"got %d devices", nDevices);
    OutputDebugString(strBfr);
    hr = pDevices->Item(0, &audioDevice);

    hr = audioDevice->Activate(IID_IAudioClient, CLSCTX_ALL,
        NULL, (void**)&pAudioClient);

    hr = pAudioClient->GetMixFormat(&pwfx);


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
    for (int c = 0; c < ((pwfx->wBitsPerSample * pwfx->nChannels) >> 3)*bufferFrameCount; c++)
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

    hr = pAudioClient->Start();  // Start playing.
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

DWORD WINAPI AudioThread(LPVOID lpParam)
{
    PAUDIODATA audioData = (PAUDIODATA)lpParam;
    UINT32 numFramesPadding, numFramesAvailable;
    while (audioData->flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {

        // Sleep for half the buffer duration.
        //Sleep((DWORD)(audioData->actualDuration / REFTIMES_PER_MILLISEC / 2));

        // See how much buffer space is available.
       audioData->audioClient->GetCurrentPadding(&numFramesPadding);

        numFramesAvailable = audioData->bufferFrameCount - numFramesPadding;

        // Grab all the available space in the shared buffer.
        audioData->renderClient->GetBuffer(numFramesAvailable, &audioData->pData);

        // Get next 1/2-second of data from the audio source.
        // The show happens here!!!
        //hr = pMySource->LoadData(numFramesAvailable, pData, &flags);

        audioData->renderClient->ReleaseBuffer(numFramesAvailable, audioData->flags);

    }
    audioData->audioClient->Stop();  // Stop playing.
    return 0;
}