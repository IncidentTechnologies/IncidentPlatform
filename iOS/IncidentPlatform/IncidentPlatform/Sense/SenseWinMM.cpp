#include "SenseWinMM.h"

//#include <time.h>

SenseWinMM::SenseWinMM(SENSE_IMP_TYPE sit) :
    m_hMidiInDevices(NULL),
    m_MidiInCaps(NULL),
    m_hMidiOutDevices(NULL),
    m_MidiOutCaps(NULL),
    m_pSenseMIDIDevices(NULL),
    m_hMidiListenerThread(NULL),
    m_InputDevices_n(0),
    m_OutputDevices_n(0),
    m_pSubscribers(NULL)
{        
    this->m_SENSE_IMP_TYPE = sit;

    // Call initialize code after SetSenseDevice
    //OpenMidiDevices();

    m_pSubscribers = new list<SenseWinMMSubscriber>();

    m_hMidiListenerThread = (HANDLE)_beginthread(SenseWinMM::MidiListnerThread, 0, (void*)(this));

    Validate();
}

void SenseWinMM::MidiListnerThread(void *pContext)
{
    SenseWinMM *pSenseWinMM = (SenseWinMM*)(pContext);
    bool fQuit = false;

    while(!fQuit)
    {
        int inDev = pSenseWinMM->m_InputDevices_n;
        int outDev = pSenseWinMM->m_OutputDevices_n;
        bool fUpdateRequired = false;
        
        if(inDev != pSenseWinMM->GetNumInputDevices())
        {
            printf("Input devices changed\n");
            pSenseWinMM->OpenInputDevices();
            fUpdateRequired = true;
        }

        if(outDev != pSenseWinMM->GetNumOutputDevices())
        {
            printf("Output devices changed\n");
            pSenseWinMM->OpenOutputDevices();
            fUpdateRequired = true;
        }

        if(fUpdateRequired)
            pSenseWinMM->NotifySubscribers();

        Sleep(10);
    }
}

SenseWinMM::~SenseWinMM()
{
    Release();
}

int SenseWinMM::GetNumInputDevices()
{ 
    int NumInDevices = midiInGetNumDevs();

    if(m_InputDevices_n != NumInDevices)       
        m_InputDevices_n = NumInDevices;
    
    return m_InputDevices_n; 
}

int SenseWinMM::GetNumOutputDevices() 
{ 
    int NumOutDevices = midiOutGetNumDevs();

    if(m_OutputDevices_n != NumOutDevices)
        m_OutputDevices_n = NumOutDevices;
    
    return m_OutputDevices_n; 
}

RESULT SenseWinMM::OpenInputDevices()
{   
    RESULT r = R_OK;

    if(m_InputDevices_n > 0)
    {
        // Set up internals
        if(m_hMidiInDevices != NULL)
            delete [] m_hMidiInDevices;
        m_hMidiInDevices = new HMIDIIN[m_InputDevices_n];

        if(m_MidiInCaps != NULL)
            delete [] m_MidiInCaps;
        m_MidiInCaps = new MIDIINCAPS[m_InputDevices_n];

        if(m_pSenseMIDIDevices != NULL)
            delete [] m_pSenseMIDIDevices;
        m_pSenseMIDIDevices = new SenseDeviceMIDI[m_InputDevices_n];

        // Initialize MidiDevices, Caps, and SenseDevices
        for(int i = 0; i < m_InputDevices_n; i++)
            m_pSenseMIDIDevices[i].SetDeviceID(i);

        for(int i = 0; i < m_InputDevices_n; i++)
        {
            midiInGetDevCaps(i, &(m_MidiInCaps[i]), sizeof(MIDIINCAPS));
            midiInOpen(&(m_hMidiInDevices[i]), i, (DWORD)MidiInProc, (DWORD)(&m_pSenseMIDIDevices[i]), CALLBACK_FUNCTION);
            midiInStart(m_hMidiInDevices[i]);
        }
    }
    else
        printf("No input midi devices found!\n");

Error:
    return r;
}

RESULT SenseWinMM::OpenOutputDevices()
{
    RESULT r = R_OK;

    if(m_OutputDevices_n > 0)
    {
        // Set up data 
        if(m_hMidiOutDevices != NULL)
            delete [] m_hMidiOutDevices;
        m_hMidiOutDevices = new HMIDIOUT[m_OutputDevices_n];

        if(m_MidiOutCaps != NULL)
            delete [] m_MidiOutCaps;
        m_MidiOutCaps = new MIDIOUTCAPS[m_OutputDevices_n];     
        
        // Initialize devices
        for(int i = 0; i < m_OutputDevices_n; i++)
        {
            midiOutGetDevCaps(i, &(m_MidiOutCaps[i]), sizeof(MIDIOUTCAPS));
            midiOutOpen(&(m_hMidiOutDevices[i]), i, (DWORD)MidiOutProc, (DWORD)(NULL), CALLBACK_FUNCTION);
        }
    }
    else
        printf("No output midi devices found!\n");

Error:
    return r;
}

RESULT SenseWinMM::OpenMidiDevices()
{
    RESULT r = R_OK;

    // Initialize input devices
    m_InputDevices_n = midiInGetNumDevs();
    CRM(OpenInputDevices(), "Failed to open input midi devices");

    // Initialize output devices
    m_OutputDevices_n = midiOutGetNumDevs();
    CRM(OpenOutputDevices(), "Failed to open output midi devices");

    Validate();
    return r;
Error:
    Invalidate();
    return r;
}

union UMidiMsg{
    DWORD dwData;
    BYTE byteData[4];
} U_MIDI_MSG;

RESULT SenseWinMM::SendSysExMessage(int id, unsigned char *pBuffer, int pBuffer_n)
{
    RESULT r = R_OK;
    UINT err;

    // Set up the MIDIHDR structure
    MIDIHDR SysEx;
    memset(&SysEx, 0, sizeof(MIDIHDR));
    SysEx.lpData = (LPSTR)(pBuffer);
    SysEx.dwBufferLength = (DWORD)(pBuffer_n);
    SysEx.dwFlags = 0;
    
    err = midiOutPrepareHeader(m_hMidiOutDevices[id], &SysEx, sizeof(MIDIHDR));
    CBRM((err == MMSYSERR_NOERROR), "SendSysExMessage::midiOutPrepareHeader failed");
    
    err = midiOutLongMsg(m_hMidiOutDevices[id], &SysEx, sizeof(MIDIHDR));
    CBRM((err == MMSYSERR_NOERROR), "SendSysExMessage::midiOutLongMsg Failed");
    
    // TODO: Should eventually be a non-spin wait
    while(MIDIERR_STILLPLAYING == midiOutUnprepareHeader(m_hMidiOutDevices[id], &SysEx, sizeof(MIDIHDR)));

Error:
    return r;
}

RESULT SenseWinMM::SendMidiMessage(int id, MidiStatusMessage msm, unsigned char channel, unsigned char data1, unsigned char data2)
{
    RESULT r = R_OK;

    CBRM((id < m_OutputDevices_n), "SenseWinMM::Output Device ID out of range!");    

    UMidiMsg uMsg;
    uMsg.byteData[0] = ((BYTE)(msm) & 0xF0) + ((unsigned char)(channel) & 0xF);
    uMsg.byteData[1] = (BYTE)(data1);
    uMsg.byteData[2] = (BYTE)(data2);
    uMsg.byteData[3] = 0;
    
    MMRESULT mr = midiOutShortMsg(m_hMidiOutDevices[id], uMsg.dwData);

Error:
    return r;
}

RESULT SenseWinMM::PrintMidiInDeviceInfo(Console *pc, int id)
{
	MIDIINCAPS mic = GetMidiInDeviceCaps(id);
	pc->PrintToOutput("MIDI Input Device id: %d\n", id);
	pc->PrintToOutput("Manufacturer Identifier: 0x%h\n", mic.wMid);
	pc->PrintToOutput("Product Identifier: 0x%h\n", mic.wPid);
	pc->PrintToOutput("Product Name: %S\n", mic.szPname);
	pc->PrintToOutput("Driver Version: 0x%h\n", mic.vDriverVersion);
	return R_OK;
}

RESULT SenseWinMM::PrintMidiOutDeviceInfo(Console *pc, int id)
{
	MIDIOUTCAPS mic = GetMidiOutDeviceCaps(id);
	pc->PrintToOutput("MIDI Output Device id: %d\n", id);
	pc->PrintToOutput("Manufacturer Identifier: 0x%h\n", mic.wMid);
	pc->PrintToOutput("Product Identifier: 0x%h\n", mic.wPid);
	pc->PrintToOutput("Product Name: %S\n", mic.szPname);
	pc->PrintToOutput("Driver Version: 0x%h\n", mic.vDriverVersion);
	return R_OK;
}

RESULT SenseWinMM::CheckUserInput()
{
    RESULT r = R_OK;

    // Callbacks perform this function automatically
    // so do we really need to do this?

    return r;
}

RESULT SenseWinMM::Release()
{
    RESULT r = R_OK;

    for(int i = 0; i < m_InputDevices_n; i++)
    {
        midiInStop(m_hMidiInDevices[i]);
        midiInClose(m_hMidiInDevices[i]);
    }

    for(int i = 0; i< m_OutputDevices_n; i++)
    {
        midiOutClose(m_hMidiOutDevices[i]);
    }

Error:
    Invalidate();
    return r;
}

void CALLBACK SenseWinMM::MidiOutProc( HMIDIOUT hMidiOut, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
{
    int id;
    midiOutGetID(hMidiOut, (LPUINT)(&id));

    printf("MIDI Output Device id:%d: ", id);

    switch(wMsg)
    {
        case MOM_OPEN:
        {
            printf("opened");
        } break;

        case MOM_CLOSE:
        {
            printf("closed");
        } break;

        case MOM_POSITIONCB:
        {
            printf("Unhandled:MOM_POSITIONCB");
        } break;

        case MOM_DONE:
        {
            printf("Unhandled:MOM_DONE");
        } break;

        default:
        {
            printf("Unknown Midi Message!\n");
        } break;
    }

    printf("\n");
}

void CALLBACK SenseWinMM::MidiInProc( HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
{
    int id;
    midiInGetID(hMidiIn, (LPUINT)(&id));

    switch(wMsg)
    {
        case MIM_OPEN:
        {
            printf("MIDI Input Device id: %d opened!\n", id);
        } break;

        case MIM_CLOSE:
        {
            printf("MIDI Input Device id: %d closed\n", id);
        }

        case MIM_DATA:
        {
            MidiStatusMessage MidiMessage = (MidiStatusMessage)((dwParam1) & 0xF0);
            int channel = (dwParam1) & 0x0F;
            unsigned char data1 = (dwParam1 >> 8) & 0xFF;
            unsigned char data2 = (dwParam1 >> 16) & 0xFF;            

            //printf("did:%d midi_msg:%x channel:%d data1:%d data2:%d time:%u\n", id, MidiMessage, channel, data1, data2, dwParam2);

            // data1 = note number -> index
            // data2 = velocity -> value
            // midimessage = status -> status
            /* FIX NEED TO IMPLEMENT SenseDeviceMidi!!!! */
            SenseDeviceMidiElement sdme;
            sdme.id = SENSE_DEVICE_ELEMENT_ID_MIDI;
            sdme.callback = NULL;
            sdme.channel = channel;
            sdme.index = data1;
            sdme.data = data2;
            sdme.time = dwParam2;
            sdme.StatusMsg = MidiMessage;

            reinterpret_cast<SenseDevice*>(dwInstance)->SetElement(data1, SENSE_DEVICE_ELEMENT_STRUCTURE, (void*)(&sdme)); 

        } break;

        case MIM_LONGDATA:
        {
            /* TODO */
        } break;

        case MIM_MOREDATA:
        {
            /* TODO */
        } break;    

        case MIM_ERROR:
        {
            /* TODO */
        } break;

        case MIM_LONGERROR:
        {
            /* TODO */
        } break;

        default:
        {
            printf("Unknown Midi Message!\n");
        } break;
    }
}

SenseDevice* SenseWinMM::GetSenseDevice( int ID )
{
    if(ID >= 0 && ID <= m_InputDevices_n)
        return (&(m_pSenseMIDIDevices[ID]));
    else
    {
        printf("Invalid SenseDeviceMidi ID!\n");         
        return NULL;
    }
}

RESULT SenseWinMM::GetSenseDeviceInfo( int ID, SENSE_DEVICE_INFO sdi, void* n_SDInfo )
{
    RESULT r = R_OK;

    n_SDInfo = NULL;

    CBRM((ID >= 0), "SenseWinMM: GetSenseInfo: Device ID cannot be less than 0");

    if(ID > m_InputDevices_n)
    {
        // While not a failure, we should not continue
        return R_OK;
    }

    switch(sdi)
    {        
        case SENSE_DEVICE_INFO_NAME:
            {
                return R_NOT_IMPLEMENTED;
            } break;

        case SENSE_DEVICE_INFO_MAX:
            {
                return R_NOT_IMPLEMENTED;
            } break;

        case SENSE_DEVICE_INFO_MIN:
            {
                return R_NOT_IMPLEMENTED;
            } break;

        case SENSE_DEVICE_INFO_RANGE:
            {
                return R_NOT_IMPLEMENTED;
            } break;

        case SENSE_DEVICE_INFO_TYPE:
            {                        
                n_SDInfo = new SENSE_DEVICE_TYPE(m_pSenseMIDIDevices[ID].GetDeviceType());
            } break;

        default:
            {
                return R_NOT_IMPLEMENTED;
            } break;
    }    

Error:
    return r;
}


float MIDINoteFrequency(int MidiNote)
{
    MidiNote -= 24;
    if(MidiNote < MIDI_MAX && MidiNote > MIDI_MIN)
    {
        return BASE_NOTE_A4 * pow(2.0f, (float)(MidiNote - 57) / 12.0f);
    }
    else
    {
        return -1.0f;
    }
}