// Sense is an Object that contains the instantaneous 
// input state.  Usually this object is owned by a
// Sense implementation and update by it.

#pragma once

#include "cmnPriv.h"
#include "valid.h"
#include "SenseDevice.h"

class Sense :
    public valid
{
public:
    Sense(SenseImp *pSenseImp)
    {
        RESULT r = R_OK;

        CNRM(pSenseImp, "Sense: Sense Implementation cannot be NULL!");
        m_pSenseImp = pSenseImp;
        m_ppSenseImps_n = 1;

        Validate();
        return;
    Error:
        Invalidate();
        return;
    }

    Sense(SenseFactory &sf, WindowImp *pWindowImp)
    {
        RESULT r = R_OK;

        CNRM(pWindowImp, "Sense: Window Implementation cannot be NULL!");

        list<SenseImp*> *pImpList = sf.MakeSense(pWindowImp);
        CNRM(pImpList, "Sense Factory returned NULL list");

        // We definitely don't want a dynamic list to be the storage for the
        // sense implementations. 
        m_ppSenseImps = pImpList->StaticBufferCopy();
        m_ppSenseImps_n = pImpList->length();
               
        //m_pSenseImp = SenseFactory.MakeSense(pWindowImp);
        m_pSenseImp = m_ppSenseImps[0];

        Validate();
        return;
    Error:
        Invalidate();
        
        /*
        if(pImpList != NULL)
        {
            delete pImpList;
            pImpList = NULL;
        }
        */
        
        return;
    }

    RESULT UpdateSenseState()
    {
        int Key;
        bool State_f;
        bool Close_f;

        for(int i = 0; i < m_ppSenseImps_n; i++)
        {        
            if(m_ppSenseImps[i] != NULL)
                m_ppSenseImps[i]->CheckUserInput();                
        }

        return SUCCEED;
    }

    RESULT RegisterInput(SENSE_DEVICE_TYPE DeviceType, int DeviceID, int index, SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;

        for(int i = 0; i < m_ppSenseImps_n; i++)
        {
            SENSE_DEVICE_TYPE sdt; 
            CRM(m_ppSenseImps[i]->GetSenseDeviceInfo(DeviceID, SENSE_DEVICE_INFO_TYPE, &sdt), "Sense: RegisterInput failed to get sense device info");                        
            if(sdt == DeviceType)
            {
                CRM(m_ppSenseImps[i]->RegisterSenseDeviceInput(DeviceID, index, Callback, pContext), "Sense: Failed to register indexed callback");
                return r;
            }
        }

        return R_FAIL;  // if we get here no compatible device was found
Error:
        return r;
    }

    RESULT RegisterDefaultInput(SENSE_DEVICE_TYPE DeviceType, int DeviceID, SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;

        for(int i = 0; i < m_ppSenseImps_n; i++)
        {
            SENSE_DEVICE_TYPE sdt; 
            CRM(m_ppSenseImps[i]->GetSenseDeviceInfo(DeviceID, SENSE_DEVICE_INFO_TYPE, &sdt), "Sense: RegisterInput failed to get sense device info");                        
            if(sdt == DeviceType)
            {
                CRM(m_ppSenseImps[i]->RegisterDefaultSenseDeviceInput(DeviceID, Callback, pContext), "Sense: Failed to register default callback");
                return r;
            }
        }

        return R_FAIL;
Error:
        return r;
    }

    static RESULT CheckKey(int key_code, char *KeyBuffer, int KeyBuffer_l)
    {
        if(key_code < KeyBuffer_l)
        {
            if(KeyBuffer[key_code] && 0x80)             
                return R_YES;            
            else             
                return R_NO;            
        }
        else
        {
            // Prevent out of bounds
            return R_KEY_CHECK_OOB;
        }
    }

    static RESULT CheckSenseDeviceStatus(int index, void *pSenseDevice, int status)
    {
        RESULT r = R_NOT_IMPLEMENTED;
        /*if(index < SENSE_DEVICE_SIZE)
        {
            if(reinterpret_cast<SenseDevice*>(pSenseDevice)->GetElementStatus(index) == status)
            {
                return R_YES;
            }
            else
            {
                return R_NO;
            }
        }
        else
        {
            // Prevent Out of Bounds
            return R_KEY_CHECK_OOB;
        } */
Error:
        return r;
    }


    RESULT CheckKey(int key_code)
    {
        RESULT r = R_NOT_IMPLEMENTED;
        //printf("not implemented!");

Error:
        return r;
    }

    ~Sense()
    {
        // release and delete the implementations
        for(int i = 0; i < m_ppSenseImps_n; i++)
        {
            if(m_ppSenseImps[i] != NULL)
            {
                m_ppSenseImps[i]->Release();
                delete m_ppSenseImps[i];
                m_ppSenseImps[i] = NULL;
            }
        }

        // delete the array
        if(m_ppSenseImps != NULL)
        {
            delete [] m_ppSenseImps;
            m_ppSenseImps = NULL;
        }
    }

private:
    SenseImp *m_pSenseImp; // used for the current implementation (or default for now)

    SenseImp **m_ppSenseImps;
    int m_ppSenseImps_n;
};