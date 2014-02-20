// Sense Input Object for the Direct Input 8 implementation
// This effectively peeks at messages from the operating system
// and updates the input state

#pragma once

#include "SenseImp.h"

// We need the windows implementation to get the hwnd
#include "WindowImp.h"

// Direct Input Headers and Libraries: dinput8.lib and dxguid.lib
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define SENSE_DIRECT_INPUT_NUM_DEVICES 2
#define SENSE_DIRECT_INPUT_DEVICE_KEYBOARD 0
#define SENSE_DIRECT_INPUT_DEVICE_MOUSE 1
#define SENSE_DIRECT_INPUT_DEVICE_KEYBOARD_NAME "Keyboard"
#define SENSE_DIRECT_INPUT_DEVICE_MOUSE_NAME "Mouse"

class SenseDirectInput8 :
    public SenseImp
{
public:
    SenseDirectInput8(SENSE_IMP_TYPE sit, WindowImp *pWindowImp);

    ~SenseDirectInput8();

    // Interface
    RESULT CheckUserInput();  
    RESULT Release();
    bool ValidSense();
    //RESULT SetSenseDevice(SenseDevice *pSenseDevice);

    int GetNumSenseDevices();
    SenseDevice* GetSenseDevice(int ID);
    RESULT GetSenseDeviceInfo(int ID, SENSE_DEVICE_INFO sdi, void* n_SDInfo);

    RESULT RegisterSenseDeviceInput(int ID, int index, SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;

        CNRM(Callback, "SenseWinMM: Cannot register null callback!");
        CBRM((ID < SENSE_DIRECT_INPUT_NUM_DEVICES), "SenseDirectInput8::Device ID out of range!");

        r = m_pSenseDevices[ID]->RegisterDeviceInputCallback(index, Callback, pContext);

Error:
        return r;
    }

    RESULT RegisterDefaultSenseDeviceInput(int ID, SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;

        CNRM(Callback, "SenseWinMM: Cannot register null callback!");
        CBRM((ID < SENSE_DIRECT_INPUT_NUM_DEVICES), "SenseDirectInput8::Device ID out of range!");
        CRM(m_pSenseDevices[ID]->RegisterDefaultDeviceInputCallback(Callback, pContext), "SenseWinMM: Failed to register default device input Callback");

Error:
        return r;
    }

    // Internal
    RESULT InitializeDirectInput();

private:
    // Direct Input Main Object
    LPDIRECTINPUT8 m_pDIObject;

    // Keyboard Object
    LPDIRECTINPUTDEVICE8 m_pDIKeyboard;     // keyboard device  
    LPDIRECTINPUTDEVICE8 m_pDIMouse;

    // Handle to Window Implementation
    WindowImp *m_pWindowImp;
    
    // Moving over to an array implementation instead
    // these are utilized for quick internal manipulation of the devices
    SenseDevice *m_pSenseDeviceKeyboard;        // Keyboard Object
    SenseDevice *m_pSenseDeviceMouse;           // Mouse Object (NOT IMPLEMENTED YET)
    
    SenseDevice *m_pSenseDevices[SENSE_DIRECT_INPUT_NUM_DEVICES];
};