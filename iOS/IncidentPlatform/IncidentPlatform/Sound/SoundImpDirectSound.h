#pragma once

// SoundmpDirectSound is a Sound Implementation utilizing the Direct Sound API 
// to pass a secondary buffer to the primary buffer and thereby to the sound card
// through the Direct Sound API.  The design of this is still up in the air

#define INITGUID

#pragma comment(lib, "Dsound.lib")
#include <DSound.h>

#include "mmreg.h"
#include <Ks.h>
#include <KsMedia.h>

#include "cmnPriv.h"
#include "SoundObj.h"
#include "SoundImp.h"

// Buffer Description Default Values

// Capabilities
// ************
// Position Notification Capability
// Get Current Position Capability
// Control Playback Frequency Capability
// Control Volume Capability
// Control Pan Capability
// Control 3d capability
// Effects Processing Capability
// Sound still play when switching to a different application
#define DEfAULT_BUFFER_FLAGS        DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS                       

#define DEFAULT_BUFFER_SIZE         1024
#define DEFAULT_RESERVED            0

// Wave Format Default Values
#define DEFAULT_WAVE_FORMAT         WAVE_FORMAT_PCM
#define DEFAULT_CHANNELS            2  
#define DEFAULT_SAMPLES_PER_SEC     44100
#define DEFAULT_AVG_BYTES_PER_SEC   DEFAULT_SAMPLES_PER_SEC * 4
#define DEFAULT_BLOCK_ALIGN         4
#define DEFAULT_BITS_PER_SAMPLE     16
#define DEFAULT_EXTRA               0



class SoundImp;
class SoundObj;

class SoundImpDirectSound :
    public SoundImp
{
public:
    SoundImpDirectSound(bool fStreaming = false);
    //SoundImpDirectSound();

    ~SoundImpDirectSound();
        
    RESULT Release();

    RESULT InitDirectSound();

    RESULT SetWaveFormatEx();
    RESULT SetDSBufferDescription();
    RESULT CreateSecondarySoundBuffer();

    // New interface stuff
    RESULT StreamBufferToImp(void *Buffer, int BufferSize, int offset);
    RESULT StreamHungry(int &offset);   // This will return an offset into which to load the next buffer

    // Interface Stuff
    RESULT PassBufferToImp(void* Buffer, int BufferSize);
    RESULT PlayBuffer(long Offset, bool Loop_f);
    RESULT StopPlayback();

    RESULT Initialize(SoundObj *pSoundObj);

    IDirectSound8* GetDirectSound(){ return m_pDSound; }
    IDirectSoundBuffer8* GetDirectSoundBuffer(){ return m_pDSoundBuffer; }

	// Position notifications
    RESULT RegisterPositionNotifications();
    RESULT CreatePositionNotification(long offset);
    RESULT CheckPositionNotifications();

    // Set get of actual implementation
    RESULT GetCurrentPlayPosition(long &Offset);
    RESULT SetCurrentPlayPosition(long Offset);
    RESULT GetCurrentWritePosition(long &Offset);
    RESULT SetCurrentWritePosition(long Offset);
    RESULT GetFrequency(long &Frequency);
    RESULT SetFrequency(long Frequency);
    RESULT GetPan(long &Pan);
    RESULT SetPan(long Pan);
    RESULT GetVolume(long &Volume);
    RESULT SetVolume(long Volume);

    SOUND_PLAYING_STATE SoundPlaying();

private:
    // Direct Sound Object
    IDirectSound8 *m_pDSound;

public:
    // Direct Sound Secondary Buffer
    IDirectSoundBuffer8 *m_pDSoundBuffer;
    
private:
    // Direct Sound Buffer Descriptor
    DSBUFFERDESC m_dsbd;

    // Direct Sound Wave Format
    //WAVEFORMATEX m_wfx;
    //WAVEFORMATEXTENSIBLE m_wfx;
    WAVEFORMATIEEEFLOATEX m_wfx;

    // Handle to the window
    HWND m_hwnd;

    // Playing on indicator flag (move to parent?)
    // bool m_SoundPlaying_f;

    // Position notification structure
    // not always defined
    IDirectSoundNotify8* m_pDirectSoundNotify;
    list<DSBPOSITIONNOTIFY> m_DSBPositionNotifyList;     
};

RESULT PrintLastWindowsError();