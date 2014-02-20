#pragma once

// Sense Object implementation for Windows Multimedia 
// Currently this mostly includes the MIDI events and messages
// however this can be expanded to other kinds of multimedia
// in the future or it can be branched to a seperate object

#include "SenseImp.h"
#include "SenseDeviceMIDI.h"
#include <math.h> // for pow
#include <process.h>

#include "Console.h"
#include "list.h"

#pragma comment(lib, "Winmm.lib")

#define BASE_NOTE_A4 440.0f
#define MIDI_MIN 0
#define MIDI_MAX 0x7F

// Generated with A4 = 440.0 Hz
// A4 = MIDI note 57
float MIDINoteFrequency(int MidiNote);

typedef void (_stdcall *SenseWinMMCallback)(void* pContext);

struct SenseWinMMSubscriber
{
    int subscriberId;
    SenseWinMMCallback fpCallback;
    void *pContext;
};  

class SenseWinMM :
    public SenseImp
{
public:
    SenseWinMM(SENSE_IMP_TYPE sit);
    ~SenseWinMM();

    RESULT OpenMidiDevices();   
    RESULT OpenOutputDevices();
    RESULT OpenInputDevices();

    int GetNumInputDevices();
    int GetNumOutputDevices();

    RESULT CheckUserInput();
    RESULT Release();
    static void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
    static void CALLBACK MidiOutProc(HMIDIOUT hMidiOut, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
    SenseDevice* GetSenseDevice(int ID);
    RESULT GetSenseDeviceInfo(int ID, SENSE_DEVICE_INFO sdi, void* n_SDInfo);

    RESULT RegisterSenseDeviceInput(int ID, int index, SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;

        CNRM(Callback, "SenseWinMM: Cannot register null callback!");
        CBRM((ID < m_InputDevices_n), "SenseWinMM::Device ID out of range!");
        CRM(m_pSenseMIDIDevices[ID].RegisterDeviceInputCallback(index, Callback, pContext), "SenseWinMM: Failed to register device input Callback");
Error:
        return r;
    }

    RESULT RegisterDefaultSenseDeviceInput(int ID, SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;

        CNRM(Callback, "SenseWinMM: Cannot register null callback!");
        CBRM((ID < m_InputDevices_n), "SenseWinMM::Device ID out of range!");   
        CRM(m_pSenseMIDIDevices[ID].RegisterDefaultDeviceInputCallback(Callback, pContext), "SenseWinMM: Failed to register default device input Callback");

Error:
        return r;
    }

    RESULT ClearDefaultSenseDeviceInput(int ID)
    {
        RESULT r = R_OK;

        CBRM((ID < m_InputDevices_n), "SenseWinMM:ClearDefaultSenseDeviceInput:Device ID out of range!");   
        CRM(m_pSenseMIDIDevices[ID].ClearDefaultDeviceInputCallback(), "SenseWinMM: Failed to clear default device input callback");

Error:
        return r;
    }   

    RESULT SendMidiMessage(int id, MidiStatusMessage msm, unsigned char channel, unsigned char data1, unsigned char data2);
    RESULT SendSysExMessage(int id, unsigned char *pBuffer, int pBuffer_n);

    int GetNumSenseDevices(){ return m_InputDevices_n; }
    bool ValidSense(){ return IsValid(); }

	MIDIINCAPS GetMidiInDeviceCaps(int id){ return m_MidiInCaps[id]; }
	MIDIOUTCAPS GetMidiOutDeviceCaps(int id){ return m_MidiOutCaps[id]; }

	RESULT PrintMidiInDeviceInfo(Console *pConsole, int id);
	RESULT PrintMidiOutDeviceInfo(Console *pConsole, int id);

    /*
    RESULT SetSenseDevice(SenseDevice *pSenseDevice)
    {
    RESULT r = R_OK;

    CRM(Release(), "SenseWinMM::SetSenseDevice");;
    m_pSenseDevice = pSenseDevice;
    CRM(OpenMidiDevices(), "SenseWinMM::OpenMidiDevices");

    Error:
    return r;
    }
    */

public:
    int Subscribe(SenseWinMMCallback fpCallback, void *pContext)
    {
        SenseWinMMSubscriber NewSubscriber;
        NewSubscriber.fpCallback = fpCallback;
        NewSubscriber.subscriberId = GetUniqueSubscriberId();
        NewSubscriber.pContext = pContext;

        m_pSubscribers->Append(NewSubscriber);

        return NewSubscriber.subscriberId;
    }

    RESULT Unsubscribe(int id)
    {
        for(list<SenseWinMMSubscriber>::iterator it = m_pSubscribers->First(); it != NULL; it++)        
            if((*it).subscriberId == id)
                return m_pSubscribers->Remove(&it);        
        return R_LIST_ITEM_NOT_FOUND;
    }

    RESULT NotifySubscribers()
    {
        for(list<SenseWinMMSubscriber>::iterator it = m_pSubscribers->First(); it != NULL; it++)  
            (*it).fpCallback((*it).pContext);
        return R_OK;
    }

private:
    static void MidiListnerThread(void *pContext); 

private:
    static int GetUniqueSubscriberId()
    { 
        static int subscriber_id = 0;
        return subscriber_id++; 
    }

private:
    // Midi Input Devices
    HMIDIIN *m_hMidiInDevices;
    MIDIINCAPS *m_MidiInCaps;
    int m_InputDevices_n;
    
    // Midi Output Devices
    HMIDIOUT *m_hMidiOutDevices;
    MIDIOUTCAPS *m_MidiOutCaps;
    int m_OutputDevices_n;

    list<SenseWinMMSubscriber> *m_pSubscribers; 

    // Responsibility for device has been pushed down to the implementations
    // This will be an array of devices now instead of only one
    // The specific device corresponding to a specific ID will be passed to the 
    // MIDI call back 
    SenseDeviceMIDI *m_pSenseMIDIDevices;  

    // New listener thread that actively looks for new devices
    HANDLE m_hMidiListenerThread;
};