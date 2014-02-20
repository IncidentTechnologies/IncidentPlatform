// Sense Device Implementation for a MIDI Device
// This will allow abstraction of a MIDI device's state and provide
// access to all of the current information regarding the MIDI device

#pragma once

#include "SenseDevice.h"
#include "MIDICommon.h"

#define SENSE_DEVICE_MIDI_SIZE 256
#define SENSE_DEVICE_MIDI_MIN 0

#define SENSE_DEVICE_MIDI_MAX (SENSE_DEVICE_MIDI_SIZE + SENSE_DEVICE_MIDI_MIN)
#define SENSE_DEVICE_MIDI_RANGE (SENSE_DEVICE_MIDI_MAX - SENSE_DEVICE_MIDI_MIN)

typedef struct SenseDeviceMidiElement
{
    SENSE_DEVICE_ELEMENT_ID id;    // should be equal to SENSE_DEVICE_ELEMENT_ID_MIDI
    int channel;
    int index;                      // MIDI indexing information
    SenseCallback callback;     // callback (NULL if not present)
    MidiStatusMessage StatusMsg;                // status (indicates on or off)
    unsigned char data;                  // data
    unsigned long time;         // timestamp   
} SENSE_DEVICE_MIDI_ELEMENT;

class SenseDeviceMIDI :
    public SenseDevice
{
public:
    SenseDeviceMIDI() :
      m_DefaultCallback(NULL),
      m_pDefaultCallbackContext(NULL),
      m_DeviceID(-1)
    {
        memset(m_SenseDeviceMidiState, 0, (sizeof(SENSE_DEVICE_MIDI_ELEMENT) * SENSE_DEVICE_MIDI_SIZE));
        for(int i = 0; i < SENSE_DEVICE_MIDI_SIZE; i++)
        {
            m_SenseDeviceMidiState[i].id = SENSE_DEVICE_ELEMENT_ID_MIDI;
        }
    }

    ~SenseDeviceMIDI(){/*empty stub*/}

    RESULT GetElement(int index, SENSE_DEVICE_ELEMENT_TYPE sdet, void* &n_pVal)
    {
        RESULT r = R_NOT_IMPLEMENTED;

        /*TODO!*/
Error:
        return r;
    }

    RESULT RegisterDeviceInputCallback(int index, SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;

        CNRM(Callback, "SenseDeviceMIDI: Cannot set the callback to null");

        // Allow to overwrite callbacks but output warning message
        if(m_SenseDeviceMidiState[index].callback != NULL)        
            printf("warning:MIDI Device callback overwrite\n");        

        m_SenseDeviceMidiState[index].callback = Callback;
        //m_SenseDeviceMidiState[index].pContext = pContext;    //ADD
        /*m_pDefaultCallbackContext = pContext;*/
Error:
        return r;
    }

    RESULT ClearDefaultDeviceInputCallback()
    {
        m_DefaultCallback = NULL;
        m_pDefaultCallbackContext = NULL;
        return R_OK;
    }

    RESULT RegisterDefaultDeviceInputCallback(SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;
        
        CNRM(Callback, "SenseDeviceMIDI: Cannot set the callback to null");

        if(m_DefaultCallback != NULL)
            printf("warning: Overwriting MIDI device default callback\n");

        m_DefaultCallback = Callback;
        m_pDefaultCallbackContext = pContext;

Error:
        return r;
    }

    RESULT SetElement(int index, SENSE_DEVICE_ELEMENT_TYPE sdet, void* pVal)
    {
        RESULT r = R_OK;
        SenseDeviceMidiElement *pElement = reinterpret_cast<SenseDeviceMidiElement*>(pVal);
        
        unsigned char uindex = (unsigned char)(index);

        if(pElement != NULL)
        {
            // Check first that the id is correct
            CBRME((pElement->id == SENSE_DEVICE_ELEMENT_ID_MIDI), "SenseDeviceMIDI::SetElement incorrect type", R_SENSE_INVALID_ELEMENT_ID);
            CBRME((uindex < SENSE_DEVICE_MIDI_MAX), "SenseDeviceMIDI::SetElement input index out of range", R_SENSE_INVALID_PARAM);
            
            // Set the m_SenseDeviceMidiState element correctly
            m_SenseDeviceMidiState[uindex].data = pElement->data;
            m_SenseDeviceMidiState[uindex].StatusMsg = pElement->StatusMsg;
            m_SenseDeviceMidiState[uindex].time = pElement->time;

            // If there is a callback registered then we pass the element to the callback
            if(m_SenseDeviceMidiState[uindex].callback != NULL)
            {
                m_SenseDeviceMidiState[uindex].callback(SENSE_DEVICE_MIDI, pVal, uindex, m_DeviceID, NULL);   // need to fix this!
            }                    
            
            if(m_DefaultCallback != NULL)
            {
                m_DefaultCallback(SENSE_DEVICE_MIDI, pVal, uindex, m_DeviceID, m_pDefaultCallbackContext);
            }
        }

Error:
        return r;
    }

    void* GetElements()
    {
        return (void*)m_SenseDeviceMidiState;
    }

    int GetDeviceMax(){ return SENSE_DEVICE_MIDI_MAX; }
    int GetDeviceMin(){ return SENSE_DEVICE_MIDI_MIN; }
    int GetDeviceRange(){ return SENSE_DEVICE_MIDI_RANGE; }
    SENSE_DEVICE_TYPE GetDeviceType(){ return SENSE_DEVICE_MIDI; }

    int GetDeviceID(){ return m_DeviceID; }
    RESULT SetDeviceID(int id){ m_DeviceID = id; return R_OK; }

private:
    SENSE_DEVICE_MIDI_ELEMENT m_SenseDeviceMidiState[SENSE_DEVICE_MIDI_SIZE];
    SenseCallback m_DefaultCallback;        // This is to allow registration of one call back, preempted by specific callbacks
    void *m_pDefaultCallbackContext;
    int m_DeviceID;     // MIDI devices have certain IDs
};
