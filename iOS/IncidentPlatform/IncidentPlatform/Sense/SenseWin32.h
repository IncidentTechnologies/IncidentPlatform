// Sense Input Object for the Win32 OS implementation
// This effectively peeks at messages from the operating system
// and updates the input state

#pragma once

#include "SenseImp.h"

#define SENSE_WIN32_NUM_DEVICES 0

#define SENSE_WIN32_DEVICE_KEYBOARD 0
#define SENSE_WIN32_DEVICE_MOUSE 1
#define SENSE_WIN32_DEVICE_KEYBOARD_NAME "Keyboard"
#define SENSE_WIN32_DEVICE_MOUSE_NAME "Mouse"

class SenseWin32 :
    public SenseImp
{
public:
    SenseWin32(SENSE_IMP_TYPE sit)
    {
        this->m_SENSE_IMP_TYPE = sit;
    }

    ~SenseWin32()
    {
        RESULT res = Release();
    }

    RESULT RegisterSenseDeviceInput(int ID, int index, SenseCallback Callback, void *pContext)
    {
        RESULT r = R_NOT_IMPLEMENTED;

Error:
        return r;
    }

    RESULT RegisterDefaultSenseDeviceInput(int ID, SenseCallback Callback, void *pContext)
    {
        RESULT r = R_NOT_IMPLEMENTED;

Error:
        return r;
    }

    RESULT CheckUserInput()
    {
        RESULT r = R_NOT_IMPLEMENTED;
        MSG msg;


        // FIX THIS!!!

        /*
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
   
            switch(msg.message)
            {
            case WM_KEYDOWN:    KeyBuffer[msg.wParam] = 0x80;
                                return SUCCEED;
                                break;

            case WM_KEYUP:      KeyBuffer[msg.wParam] = 0x00;
                                return SUCCEED;
                                break;

            /*case WM_QUIT:       *pClose_f = true;
                                return FAIL;
                                break;

            default:    DispatchMessage(&msg);
                        return FAIL;
            }
        }
        */

Error:
        return r;
    }

    RESULT Release()
    {        
        return R_OK;
    }

    /*
    RESULT SetSenseDevice(SenseDevice *pSenseDevice)
    {
        RESULT r = R_OK;

        m_pSenseDevice = pSenseDevice;

Error:
        return r;
    }
    */

    bool ValidSense()
    {
        return IsValid();
    }     

    int GetNumSenseDevices(){ return SENSE_WIN32_NUM_DEVICES; }
    SenseDevice* GetSenseDevice(int ID)
    {
        // No sense device implementation here yet (FIX!!)
        return NULL;
    }

    RESULT GetSenseDeviceInfo(int ID, SENSE_DEVICE_INFO sdi, void* n_SDInfo)
    {
        RESULT r = R_NOT_IMPLEMENTED;

Error:
        return r;
    }
};