#pragma once

// Sense device is effectively a table of SenseDeviceElements (which are also defined here)
// and create a large array (256 for the time being) of possible inputs and their effective
// statuses

#include <string.h>

typedef enum SenseDeviceType
{
    SENSE_DEVICE_KEYBOARD,
    SENSE_DEVICE_MOUSE,
    SENSE_DEVICE_MIDI,
    SENSE_DEVICE_INVALID        // Invalid Device
} SENSE_DEVICE_TYPE;

typedef enum SenseDeviceElementId
{
    SENSE_DEVICE_ELEMENT_ID_CHARACTER,
    SENSE_DEVICE_ELEMENT_ID_MOUSE,      // this is temporary and is the overall mouse state in one struct
    SENSE_DEVICE_ELEMENT_ID_AXIS,
    SENSE_DEVICE_ELEMENT_ID_BUTTON,
    SENSE_DEVICE_ELEMENT_ID_MIDI,
    SENSE_DEVICE_ELEMENT_ID_INVALID
} SENSE_DEVICE_ELEMENT_ID;

typedef enum SenseDeviceElementType
{
    SENSE_DEVICE_ELEMENT_INDEX,
    SENSE_DEVICE_ELEMENT_STATUS,
    SENSE_DEVICE_ELEMENT_VALUE,
    SENSE_DEVICE_ELEMENT_TIME,
    SENSE_DEVICE_ELEMENT_NAME,
    SENSE_DEVICE_ELEMENT_STRUCTURE      // This passes the device specific structure
} SENSE_DEVICE_ELEMENT_TYPE;

typedef enum SenseDeviceInfo
{
    SENSE_DEVICE_INFO_NAME,
    SENSE_DEVICE_INFO_MAX,
    SENSE_DEVICE_INFO_MIN,
    SENSE_DEVICE_INFO_RANGE,
    SENSE_DEVICE_INFO_TYPE,
    SENSE_DEVICE_INFO_INVALID    
} SENSE_DEVICE_INFO;

// The client application can register callbacks for different input
// values.  The callback is called whenever the state of the specific
// element changes (the element ID will be in a special enum for each
// specific SENSE_DEVICE_TYPE.  The SENSE_DEVICE_TYPE will aid the 
// callback in knowing how to parse the input.  The int will designate
// the index of the element so that multiple elements may be registered
// with the same callback

typedef void* SenseDeviceElement;
typedef void (_stdcall *SenseCallback)(int sense_device_type, SenseDeviceElement, int index, int deviceID, void* pContext);               

class SenseDevice
{
public:
    SenseDevice(){/*empty stub*/}
    ~SenseDevice(){/*empty stub*/}

    virtual RESULT SetElement(int index, SENSE_DEVICE_ELEMENT_TYPE sdet, void* pVal) = 0;
    virtual RESULT GetElement(int index, SENSE_DEVICE_ELEMENT_TYPE sdet, void* &n_pVal) = 0;
    virtual void* GetElements() = 0;    

    virtual int GetDeviceRange() = 0;
    virtual SENSE_DEVICE_TYPE GetDeviceType() = 0;
    virtual int GetDeviceMin() = 0;
    virtual int GetDeviceMax() = 0;

    virtual RESULT RegisterDeviceInputCallback(int index, SenseCallback Callback, void *pContext) = 0;
    virtual RESULT RegisterDefaultDeviceInputCallback(SenseCallback Callback, void *pContext) = 0;          // The default callback can be used to handle arbitary data, preempted by the indexed callbacks

    virtual int GetDeviceID() = 0;
    virtual RESULT SetDeviceID(int id) = 0;
};