#include "SoundImpDirectSound.h"

#include <math.h>

typedef enum
{
    WAVEFORM_SINE,
    WAVEFORM_SQUARE,
    WAVEFORM_SAW,
    WAVEFORM_TRIANGLE
} WAVEFORM_TYPE;

// Utility Function
RESULT FillBufferWaveForm(short *pBuffer, float Frequency_hz, WAVEFORM_TYPE wft)
{
    RESULT r = R_OK;

    short *cur = pBuffer;
    for(int i = 0; i < 44100; i++)
    {
        float temp = 1.0f * sinf(float(i)/(20.0f));
        float output = (short)(temp * 0x7FFF);
        cur[0] = output;
        cur[1] = output;
        cur += 2;
    }

Error:
    return r;
}

// SoundImpDirectSound
//
// Note: this will delete the memory passed to it since the sound buffer should only
// exist in one place at a time.  Due to this it is very important to only use this 
// function through the interface defined between SoundObj and SoundImp since the SoundObj
// implementation will copy all of the different pieces of the buffer to one location
// before passing them to this function
RESULT SoundImpDirectSound::PassBufferToImp(void* d_Buffer, int BufferSize)
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;
    void* WriteBuffer;
    DWORD dwLength;

    // Lock the buffer
    hr = m_pDSoundBuffer->Lock( 0,                      // offset
                                BufferSize,             // size in bytes of the portion of the buffer to lock
                                &WriteBuffer,           // Address of variable that gets address of buffer portion
                                &dwLength,              // Pointer to size of buffer portion
                                NULL,                   // Pointer to second buffer portion
                                NULL,                   // Pointer to size of second buffer portion
                                DSBLOCK_ENTIREBUFFER    // Flags
                              );
    CHRM(hr, "SoundImpDirectSound::Lock");

    // Copy the buffer over
    //memcpy(WriteBuffer, d_Buffer, dwLength);

	memcpy(WriteBuffer, d_Buffer, BufferSize);
    CNRM(WriteBuffer, "Could not copy over buffer, likely memory issue");

    // Unlock the buffer
    hr = m_pDSoundBuffer->Unlock(   WriteBuffer,            // Address of variable that gets address of buffer portion
                                    dwLength,               // Pointer to size of buffer portion
                                    NULL,                   // Pointer to second buffer portion
                                    NULL                    // Pointer to size of second buffer portion
                                );
    CHRM(hr, "SoundImpDirectSound::Unlock");

Error:
    delete d_Buffer;
    return r;
}

RESULT SoundImpDirectSound::StreamHungry(int &offset)
{
    long CurrentByte;
	GetCurrentPlayPosition(CurrentByte);
	long CurrentSample = (CurrentByte << 3) / m_pSoundObj->GetBitsPerSample();

	// Set boundaries
	long leftBoundary = m_CurrentWritePosition + m_pSoundObj->GetSampleCount();
	if(leftBoundary >= m_pSoundObj->GetSampleCount() * SOUND_STREAMING_BUFFER_DIVISIONS)
		leftBoundary = 0;

	long rightBoundary = leftBoundary + m_pSoundObj->GetSampleCount(); 

	if(CurrentSample > leftBoundary && CurrentSample < rightBoundary)
	{		
		offset = (int)CurrentSample;
		return R_SOUND_STREAM_HUNGRY;
	}
	else
		return R_SOUND_STREAM_NOT_HUNGRY;
}

RESULT SoundImpDirectSound::StreamBufferToImp(LPVOID d_Buffer, int BufferSize, int offset)
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;    
    
    LPVOID WriteBuffer1 = NULL;
    DWORD dwLength1;
    LPVOID WriteBuffer2 = NULL;
    DWORD dwLength2 = 0;

    CBRM(m_fStreaming, "Cannot stream to a nonstremaing implementation");

    // Lock the buffer
	DWORD	dwWritePos =  ((DWORD)(m_CurrentWritePosition * m_pSoundObj->GetBitsPerSample()) >> 3);
    hr = m_pDSoundBuffer->Lock( dwWritePos,                      // offset
                                BufferSize,             // size in bytes of the portion of the buffer to lock
                                &WriteBuffer1,           // Address of variable that gets address of buffer portion
                                &dwLength1,              // Pointer to size of buffer portion
                                &WriteBuffer2,                   // Pointer to second buffer portion
                                &dwLength2,                   // Pointer to size of second buffer portion
                                NULL                    // Flags
                                );
    CHRM(hr, "SoundImpDirectSound::Lock");

    // Copy the buffer over
	CNRM(WriteBuffer1, "Could not copy over buffer, likely memory issue");
    memcpy(WriteBuffer1, d_Buffer, dwLength1);
	
	/*
	Buffer should be split into equal parts!!
    if(WriteBuffer2 != NULL)
    {
        memcpy(WriteBuffer2, (void*)((int)d_Buffer + dwLength1), dwLength2);    
    }
	*/
    
    // Unlock the buffer
    hr = m_pDSoundBuffer->Unlock(   WriteBuffer1,            // Address of variable that gets address of buffer portion
                                    dwLength1,               // Pointer to size of buffer portion
                                    WriteBuffer2,                   // Pointer to second buffer portion
                                    dwLength2                    // Pointer to size of second buffer portion
                                    );
    CHRM(hr, "SoundImpDirectSound::Unlock");

	// Update the circular buffer position
	m_LastWritePosition = m_CurrentWritePosition;
	m_CurrentWritePosition += m_pSoundObj->GetSampleCount();
	if(m_CurrentWritePosition >= m_pSoundObj->GetSampleCount() * SOUND_STREAMING_BUFFER_DIVISIONS)
		m_CurrentWritePosition = 0;


Error:
    delete d_Buffer;
    d_Buffer = NULL;
    return r;
}

RESULT SoundImpDirectSound::PlayBuffer(long Offset, bool Loop_f)
{
    RESULT r = R_OK;

    if(!SoundPlaying())
    {    
        // Set The Position to 0
        CHRM(m_pDSoundBuffer->SetCurrentPosition(Offset), "SoundImpDirectSound::PlayBuffer::SetCurrentPosition");
        CHRM(m_pDSoundBuffer->Play(0, 0, (Loop_f) ? DSBPLAY_LOOPING : 0), "SoundImpDirectSound::PlayBuffer");
        //SetSoundPlaying(true);
    }
    else
    {
        printf("Sound already playing!\n");
    }

Error:
    return r;
}

RESULT SoundImpDirectSound::StopPlayback()
{
    RESULT r = R_OK;

    if(SoundPlaying())
    {    
        CHRM(m_pDSoundBuffer->Stop(), "SoundImpDirectSound::StopPlayBack::Stop");
        //SetSoundPlaying(false);
    }

Error:
    return r;
}

RESULT SoundImpDirectSound::RegisterPositionNotifications()
{
    RESULT r = R_OK;
    int s = m_DSBPositionNotifyList.Size();
    // Convert the notification list into an array
    DSBPOSITIONNOTIFY *DSBArray = new DSBPOSITIONNOTIFY[m_DSBPositionNotifyList.Size()];
    
    int i = 0;

    if(m_pDirectSoundNotify == NULL)
    {        
        // Create the IDirectSoundNotify structure
        CHRM(m_pDSoundBuffer->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&m_pDirectSoundNotify), "CreatePositionNotification::QueryInterface");
    }    

    memset(DSBArray, 0, sizeof(DSBPOSITIONNOTIFY) * m_DSBPositionNotifyList.Size());

    for(list<DSBPOSITIONNOTIFY>::iterator it = m_DSBPositionNotifyList.First(); it != NULL; it++)
    {
        DSBArray[i].dwOffset = static_cast<DSBPOSITIONNOTIFY>(*it).dwOffset;
        DSBArray[i].hEventNotify = static_cast<DSBPOSITIONNOTIFY>(*it).hEventNotify;
        i++;
    }

    CBRM(i == m_DSBPositionNotifyList.Size(), "RegisterPositionNotifications Array size doesn't match list!");
    CHRM(m_pDirectSoundNotify->SetNotificationPositions(i, DSBArray), "SoundImpDirectSound::RegisterPositionNotifications::SetNotificationPositions");

	// release the notification interface
	m_pDirectSoundNotify->Release();
	m_pDirectSoundNotify = NULL;

Error:
    return r;
}

// Check Position Notifications will check to see if any of the
// position notifications that we registered have been reached
// This is a non blocking function and must be called to update 
// itself.  Generally this should be put in some kind of update loop 

// This function needs to be updated to allow for callbacks as well
RESULT SoundImpDirectSound::CheckPositionNotifications()
{
	RESULT r = R_OK;

	for(list<DSBPOSITIONNOTIFY>::iterator it = m_DSBPositionNotifyList.First(); it != NULL; it++)
	{
		HRESULT hr = WaitForSingleObject(static_cast<DSBPOSITIONNOTIFY>(*it).hEventNotify, 0);
		if(hr == WAIT_OBJECT_0)
		{
			//printf("Position reached offset %d\n", reinterpret_cast<DSBPOSITIONNOTIFY*>(*it)->dwOffset);
			printf("Position reached %d\n", static_cast<DSBPOSITIONNOTIFY>(*it).dwOffset);

		}
		if(hr == WAIT_FAILED)
		{            
			PrintLastWindowsError();
		}
	}


Error:
	return r;
}

RESULT SoundImpDirectSound::CreatePositionNotification(long offset)
{
    RESULT r = R_OK;
    DSBPOSITIONNOTIFY DSBPositionNotify;

    DSBPositionNotify.dwOffset = offset;
    DSBPositionNotify.hEventNotify = CreateEvent(NULL,  // Security Attributes
                                                 FALSE, // Manual Reset
                                                 FALSE, // Initial State
                                                 NULL   // Name
                                                );

    // Add the position notification to the list
    // so that we can access the event and know about 
    // it later (don't forget to delete it all later)
    m_DSBPositionNotifyList.Append(DSBPositionNotify);

Error:
    return r;
}

RESULT PrintLastWindowsError()
{
	RESULT r = R_OK;

	LPTSTR pszMessage;
	DWORD dwLastError = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwLastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&pszMessage,
		0, NULL );

	// Display the error message and exit the process
	wprintf(L"failed with error %d: %s\n", dwLastError, pszMessage);

Error:
	return r;
}

RESULT SoundImpDirectSound::Initialize(SoundObj *pSoundObj)
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;

    CRM(SetSoundObj(pSoundObj), "Could not send SoundObj in SoundImpDirectSound");
    CNRM(m_pSoundObj, "Sound Object is NULL!");
   
    // Not sure if any of this is needed (idanz)
    HWND hwndConsole = GetConsoleWindow();
    if(hwndConsole == NULL)
    {
        // allocate a new console
        AllocConsole();
        hwndConsole = GetConsoleWindow();
        //AttachConsole(ATTACH_PARENT_PROCESS);
        //freopen( "CON", "w", stdout );
        printf("hello!");
    }
    m_hwnd = GetTopWindow(NULL);
    CHM(m_hwnd, "hwnd is null!");

    // First initializes the direct sound object
    CRM(InitDirectSound(), "SoundImpDirectSound::InitDirectSound");

    // Set the wave format 
    CRM(SetWaveFormatEx(), "SoundImpDirectSound::SetWaveFormatEx");

    // Set up the buffer description (should be done after wave format although
    // likely will still work if done after)
    CRM(SetDSBufferDescription(), "SoundImpDirectSound::SetDSBufferDescription");

    // Create the secondary buffer!
    CRM(CreateSecondarySoundBuffer(), "SoundImpDirectSound::CreateSecondarySoundBuffer");  

    // Memory set the buffer to 0
    void* WriteBuffer = NULL;
    DWORD dwLength;

    // Lock the buffer
    hr = m_pDSoundBuffer->Lock( 0,                      // offset
                                NULL,                   // size in bytes of the portion of the buffer to lock
                                &WriteBuffer,           // Address of variable that gets address of buffer portion
                                &dwLength,              // Pointer to size of buffer portion
                                NULL,                   // Pointer to second buffer portion
                                NULL,                   // Pointer to size of second buffer portion
                                DSBLOCK_ENTIREBUFFER    // Flags
                                );
    CHRM(hr, "SoundImpDirectSound::Lock");

	// If streaming fill data from the SoundObj
	if(m_fStreaming)
	{
		void *pTempBuffer = NULL;
		long numSamples = (dwLength / m_pSoundObj->GetBitsPerSample()) << 3;
		CRM(m_pSoundObj->GetSamples(numSamples, pTempBuffer), "Initialize: GetSamples failed");		
		DWORD tempLength = (numSamples * m_pSoundObj->GetBitsPerSample()) >> 3;
		memcpy(WriteBuffer, pTempBuffer, dwLength);
	}
	else
	{
		// Copy the buffer over
		memset(WriteBuffer, 0, dwLength);
	}

    // Unlock the buffer
    hr = m_pDSoundBuffer->Unlock(   WriteBuffer,            // Address of variable that gets address of buffer portion
                                    dwLength,               // Pointer to size of buffer portion
                                    NULL,                   // Pointer to second buffer portion
                                    NULL                    // Pointer to size of second buffer portion
                                    );
    CHRM(hr, "SoundImpDirectSound::Unlock");

    // Initialize the streaming buffer (should be 4 times the size)
    if(m_fStreaming)
    {
		m_LastWritePosition = m_pSoundObj->GetSampleCount() * (SOUND_STREAMING_BUFFER_DIVISIONS - 1);
		//m_CurrentWritePosition = m_pSoundObj->GetSampleCount();
		m_CurrentWritePosition = 0;
    }

Error:
    return r;
}

SoundImpDirectSound::SoundImpDirectSound(bool fStreaming) :
    SoundImp(fStreaming),
    m_pDSound(NULL),
    m_pDSoundBuffer(NULL),
    m_hwnd(NULL),
    m_pDirectSoundNotify(NULL)
    //m_buffer(NULL)
    //m_pSoundObj(pSoundObj)      // This is used to set the wave format and buffer descriptors (mostly wave format)
    //m_pSoundObj(NULL)
{
    // Empty Stub for now
    // all of the important stuff goes into Initialize which must be called after 
    // SoundImp create or before play is called.
}

SoundImpDirectSound::~SoundImpDirectSound()
{
    RESULT r = Release();
}

RESULT SoundImpDirectSound::Release()
{
    RESULT r = R_OK;
    
    if(m_pDSoundBuffer)
    {
        m_pDSoundBuffer->Stop();
        m_pDSoundBuffer->Release();
        m_pDSoundBuffer = NULL;
    }

    if(m_pDSound)
    {        
        m_pDSound->Release();
        m_pDSound = NULL;
    }

    // CBRM(FreeConsole(), "Could not free console!");

Error:
    return r;
}

RESULT SoundImpDirectSound::InitDirectSound()
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;

     // First Create the DirectSoundObject
    hr = DirectSoundCreate8(NULL, &m_pDSound, NULL);
	CHRM(hr, "InitDirectSound: DirectSoundCreate8 failed");

    hr = CoInitializeEx(NULL, 0);
    if(hr == S_FALSE)
        DEBUG_LINEOUT("Warning: SoundImpDirectSound:CoInitializeEx already called");
    else if(hr == RPC_E_CHANGED_MODE)
        DEBUG_LINEOUT("Warning: A previous call to CoInitializeEx specified the concurrency model for this thread as multithread apartment (MTA). This could also indicate that a change from neutral-threaded apartment to single-threaded apartment has occurred.");

    CHRM(m_pDSound->SetCooperativeLevel(m_hwnd, DSSCL_EXCLUSIVE), "SetCooperativeLevel");

Error:
    return r;
}

RESULT SoundImpDirectSound::SetWaveFormatEx()
{
    RESULT r = R_OK;

    memset(&m_wfx, 0, sizeof(m_wfx));

    m_wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;

    m_wfx.Format.nChannels = m_pSoundObj->GetChannels(); 
    m_wfx.Format.nSamplesPerSec = m_pSoundObj->GetSampleRate();
    m_wfx.Format.nAvgBytesPerSec = m_pSoundObj->GetAvgBytesPerSecond(); 
    m_wfx.Format.nBlockAlign = m_pSoundObj->GetBlockAlign();
    m_wfx.Format.wBitsPerSample = m_pSoundObj->GetBitsPerSample();

    m_wfx.Format.cbSize = 22;
    
    m_wfx.Samples.wValidBitsPerSample = m_pSoundObj->GetBitsPerSample();

    m_wfx.dwChannelMask = 0;

    switch(m_pSoundObj->GetCompressionCode())
    {
        case COMPRESSION_CODE_PCM: m_wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM; break;
        case COMPRESSION_CODE_PCM_FLOAT: m_wfx.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT; break;

        default: m_wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    }

Error:
    return r;
}

RESULT SoundImpDirectSound::SetDSBufferDescription()
{
    RESULT r = R_OK;

    memset(&m_dsbd, 0, sizeof(DSBUFFERDESC));

    m_dsbd.dwSize = (DWORD)sizeof(DSBUFFERDESC);
    m_dsbd.dwFlags = DEfAULT_BUFFER_FLAGS;
    m_dsbd.dwBufferBytes = m_pSoundObj->GetSampleCount() * (m_pSoundObj->GetBitsPerSample() / 8);
    
    // If streaming is on then the buffer needs to be that much bigger
    if(m_fStreaming)
        m_dsbd.dwBufferBytes *= SOUND_STREAMING_BUFFER_DIVISIONS;
    
    m_dsbd.dwReserved = DEFAULT_RESERVED;
    
    m_dsbd.lpwfxFormat = (WAVEFORMATEX*)&m_wfx;

Error:
    return r;
}

RESULT SoundImpDirectSound::CreateSecondarySoundBuffer()
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;

    hr = m_pDSound->CreateSoundBuffer(  &m_dsbd, (LPDIRECTSOUNDBUFFER*)&m_pDSoundBuffer, NULL ); 
    CHRM(hr, "CreateSecondarySoundBuffer: Failed to create the sound buffer!");

Error:
    return r;
}

// Returns the offset in BYTES
RESULT SoundImpDirectSound::GetCurrentPlayPosition(long &Offset)
{
    RESULT r = R_OK;

    CHRM(m_pDSoundBuffer->GetCurrentPosition((DWORD*)(&Offset), NULL), "IDirectSoundBuffer8::GetCurrentPosition");

Error:
    return r;
}

RESULT SoundImpDirectSound::SetCurrentPlayPosition(long Offset)
{
    RESULT r = R_OK;

    CHRM(m_pDSoundBuffer->SetCurrentPosition(Offset), "IDirectSoundBuffer8::SetCurrentPosition");

Error:
    return r;
}

RESULT SoundImpDirectSound::GetCurrentWritePosition(long &Offset)
{
    RESULT r = R_OK;

    CHRM(m_pDSoundBuffer->GetCurrentPosition(NULL, (DWORD*)(&Offset)), "IDirectSoundBuffer8::GetCurrentPosition");

Error:
    return r;
}

RESULT SoundImpDirectSound::SetCurrentWritePosition(long Offset)
{
    RESULT r = R_OK;

    /*FIX*/

Error:
    return r;
}

RESULT SoundImpDirectSound::GetFrequency(long &Frequency)
{
    RESULT r = R_OK;

    CHRM(m_pDSoundBuffer->GetFrequency((DWORD*)(&Frequency)), "IDirectSoundBuffer8::GetFrequency");

Error:
    return r;
}

RESULT SoundImpDirectSound::SetFrequency(long Frequency)
{
    RESULT r = R_OK;

    CHRM(m_pDSoundBuffer->SetFrequency(Frequency), "IDirectSoundBuffer8::SetFrequency");

Error:
    return r;
}

RESULT SoundImpDirectSound::GetPan(long &Pan)
{
    RESULT r = R_OK;

    CHRM(m_pDSoundBuffer->GetPan((LONG*)(&Pan)), "IDirectSoundBuffer8::GetPan");

Error:
    return r;
}

RESULT SoundImpDirectSound::SetPan(long Pan)
{
    RESULT r = R_OK;

    CHRM(m_pDSoundBuffer->SetPan(Pan), "IDirectSoundBuffer8::SetPan");

Error:
    return r;
}

RESULT SoundImpDirectSound::GetVolume(long &Volume)
{
    RESULT r = R_OK;

    HRESULT hr = m_pDSoundBuffer->GetVolume((LONG*)(&Volume));

    if(hr == DSERR_CONTROLUNAVAIL) printf("Error: Control not available!\n");
    if(hr == DSERR_INVALIDPARAM) printf("Error: Invalid Param\n");
    if(hr == DSERR_PRIOLEVELNEEDED) printf("Error: Privledge level needed!\n");

    CHRM(hr, "IDirectSoundBuffer8::GetVolume");

Error:
    return r;
}

RESULT SoundImpDirectSound::SetVolume(long Volume)
{
    RESULT r = R_OK;

    CHRM(m_pDSoundBuffer->SetVolume(Volume), "IDirectSoundBuffer8::SetVolume");

Error:
    return r;
}

SOUND_PLAYING_STATE SoundImpDirectSound::SoundPlaying()
{
    SOUND_PLAYING_STATE sps = SOUND_UNDETERMINED;
    DWORD dwStatus;
    HRESULT hr = S_OK;
    RESULT r = R_OK;
    
    CNRM(m_pDSoundBuffer, "SoundImpDirectSound:SoundPlaying:Cannot get sound playing status since sound buffer is NULL");

    hr = m_pDSoundBuffer->GetStatus(&dwStatus);

    if(hr == DSERR_INVALIDPARAM) 
    {
        printf("Error in IDirectSoundBuffer8::GetStatus!\n");
        return SOUND_UNDETERMINED;
    }
    else if(dwStatus & DSBSTATUS_PLAYING > 0) 
    {
        return SOUND_PLAYING;
    }

Error:
    return (r == R_OK) ? SOUND_NOT_PLAYING : SOUND_UNDETERMINED;
}