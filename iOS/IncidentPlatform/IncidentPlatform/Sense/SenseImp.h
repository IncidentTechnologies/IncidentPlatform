// General Sense Implementation Base Class

#pragma once

#include "cmnPriv.h"

#include "SenseDevice.h"
#include "valid.h"

typedef enum SenseImpType
{
    SENSE_DEFAULT       = 0x00000001,
    SENSE_WIN32         = 0x00000002,
    SENSE_DIRECTINPUT8  = 0x00000004,
    SENSE_WINMM         = 0x00000008
} SENSE_IMP_TYPE;

class SenseImp : protected valid
{
public:
    SenseImp() :
      valid(false)
    {/*empty stub*/}

    ~SenseImp()
    {/*empty stub*/}

    // Interface Functionality
    virtual RESULT CheckUserInput() = 0;
    virtual bool ValidSense() = 0;
    virtual RESULT Release() = 0;

    // DEPRECATED SenseImps are responsible for their own devices using the devices interface
    // virtual RESULT SetSenseDevice(SenseDevice *pSenseDevice) = 0;  

    // SenseDeviceInterface
    virtual int GetNumSenseDevices() = 0;
    virtual SenseDevice* GetSenseDevice(int ID) = 0;
    virtual RESULT GetSenseDeviceInfo(int ID, SENSE_DEVICE_INFO sdi, void* n_SDInfo) = 0;

    virtual RESULT RegisterSenseDeviceInput(int ID, int index, SenseCallback Callback, void *pContext) = 0;
    virtual RESULT RegisterDefaultSenseDeviceInput(int ID, SenseCallback Callback, void *pContext) = 0;  // Will register a default callback for a specific device, overridden by indexed callback

public:
    SENSE_IMP_TYPE m_SENSE_IMP_TYPE;
};