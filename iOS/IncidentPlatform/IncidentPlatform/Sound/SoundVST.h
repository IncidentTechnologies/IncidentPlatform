#pragma once

// The SoundVST object is effectively a sound object which will load up
// a VST plug in and provide access to it.  Also it will allow the opening
// and closing of the VST editor if one exists as well as pass the data to 
// direct sound to be heard by the user

// This class is built as a windows specific class at the moment and eventually a VSTImp parent
// class should be used for platform dependent code

#include "SoundObj.h"
#include "SoundImpFactory.h"
#include "pluginterfaces/vst2.x/aeffectx.h"
#include "MIDICommon.h"
#include "Console.h"

// Should make these adjustable and dynamic as well
#define SOUND_VST_DEFAULT_SAMPLES 1024 * 2
#define SOUND_VST_DEFAULT_BITSPERSAMPLE (sizeof(float) * 8)
#define SOUND_VST_DEFAULT_CHANNELS 1
#define SOUND_VST_DEFAULT_SAMPLESPERSEC 44100
#define SOUND_VST_DEFAULT_WAVE_FORMAT WAVE_FORMAT_PCM
#define SOUND_VST_DEFAULT_AVG_BYTES_PER_SEC SOUND_VST_DEFAULT_SAMPLESPERSEC * (SOUND_VST_DEFAULT_BITSPERSAMPLE >> 3) * SOUND_VST_DEFAULT_CHANNELS
#define SOUND_VST_DEFAULT_BLOCK_ALIGN 4

#define MAX_VST_STRING_LENGTH 250
//#define SOUND_VST_BUFFER_COUNT 2

// Callback functions for VST and windows specific stuff
VstIntPtr VSTCALLBACK HostCallback (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt); 
INT_PTR CALLBACK EditorProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef AEffect* (*PluginEntryProc) (audioMasterCallback audioMaster);


struct MyDLGTEMPLATE: DLGTEMPLATE
{
    WORD ext[3];
    MyDLGTEMPLATE ()
    {
        memset (this, 0, sizeof (*this));
    };
};


class SoundVST : 
    public SoundObj
{
public:
    SoundVST(SoundImpFactory* pSIF, char *pszVSTFilename);

    // Alternative constructor for when the sound implementation does not belong to the SoundObject
    // this is pertinent in the plugin scenario where the plugin will regulate and control the SoundImp
    SoundVST(SoundImp* pSI, char *pszVSTFilename);
    ~SoundVST();

    RESULT Initialize();

    // ***************************************************************
    // Sound Object Interface
    // ***************************************************************
    // These Interfaces are defined so that a generic implementation
    // can get this information out of the data.
    COMPRESSION_CODE GetCompressionCode(){ return COMPRESSION_CODE_PCM_FLOAT; }
    int GetChannels(){ return SOUND_VST_DEFAULT_CHANNELS; }
    int GetSampleRate(){ return SOUND_VST_DEFAULT_SAMPLESPERSEC; }
    long GetAvgBytesPerSecond(){ return SOUND_VST_DEFAULT_AVG_BYTES_PER_SEC; }
    int GetBlockAlign(){ return SOUND_VST_DEFAULT_BLOCK_ALIGN; }
    int GetBitsPerSample(){ return SOUND_VST_DEFAULT_BITSPERSAMPLE; }

    // This will return the total number of samples in the sound
    long GetSampleCount()
    {
        return m_BufferSize;
    }

    void Print()
    { 
        printf("VST Plugin Sound Object\n"); 
    }

    RESULT GetSoundStatus(SoundStatus* &n_pSS)
    {
        n_pSS = NULL;
        return R_NOT_IMPLEMENTED;
    }

    // VST Sound should always be on?
    bool SoundPlaying()
    {
        return m_pSoundImp->SoundPlaying();
    }

	RESULT GetSamples(long &SampleCount, void * &n_pVSTBuffer);

private:
    RESULT SendVstEvent(VstEvent *vstEvent)
    {
        RESULT r = R_OK;
        
        VstEvents vstEvents;
        memset(&vstEvents, 0, sizeof(VstEvents));
        vstEvents.numEvents = 1;
        vstEvents.events[0] = vstEvent;

        // Send the event to the plug-in
        m_pAEffect->dispatcher(m_pAEffect, effProcessEvents, 0, 0, (void*)(&vstEvents), 0);

Error:
        return r;
    }

    RESULT SendMidiEvent(MIDI_STATUS_MESSAGE msm, unsigned char data1 = 0x00, unsigned char data2 = 0x00, bool fRealtime = false)
    {
        RESULT r = R_OK;

        // Lets try sending a MIDI message!
        VstMidiEvent midiEvent;
        memset(&midiEvent, 0, sizeof(VstMidiEvent));
        midiEvent.type = kVstMidiType;
        midiEvent.byteSize = sizeof(VstMidiEvent);
        midiEvent.flags = (fRealtime) ? kVstMidiEventIsRealtime : 0;
        midiEvent.midiData[0] = msm;   // note on
        midiEvent.midiData[1] = data1;   // Middle C
        midiEvent.midiData[2] = data2;   // Full velocity (127)

        CRM(SendVstEvent((VstEvent*)(&midiEvent)), "SoundVST: Failed to send VST Event");

Error:
        return r;
    }

public:
    RESULT SendNoteOn(unsigned char note, unsigned char velocity)
    {
        return SendMidiEvent(MIDI_NOTE_ON, note, velocity, true);
    }

    RESULT SendNoteOff(unsigned char note, unsigned char velocity)
    {
        return SendMidiEvent(MIDI_NOTE_OFF, note, velocity, true);
    }

    RESULT HandleEditorMessage(MSG msg)
    {
        RESULT r = R_OK;

        if (!IsDialogMessage(m_hwndVSTEditor, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

Error:
        return r;
    }

    /*
    static void CALLBACK TimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
    {

    }
    */


    RESULT Update();

    // ***************************************************************
    AEffect *GetAEffect(){ return m_pAEffect; }

    // ***************************************************************

private:
    // VST members
    AEffect *m_pAEffect;
    HWND m_hwndVSTEditor;
    HMODULE m_hmoduleVSTPlugin;

    char m_pszEffectName[MAX_VST_STRING_LENGTH];
    char m_pszVendorName[MAX_VST_STRING_LENGTH];
    char m_pszProductName[MAX_VST_STRING_LENGTH];

    char *m_pszFilename;
    int m_BufferSize;

    // We will be using a multi-buffer design so that while the sound is playing
    // we can retrieve the next buffer section
    // 

    float *m_pFloatBuffer;
    //int m_CurrentBuffer;
    bool m_fBufferReady;

    SoundImp *m_pSoundImp;
    SoundImpFactory *m_pSoundImpFactory;
};

RESULT TestSoundVST(Console *pConsole, char *pszVSTFilename);