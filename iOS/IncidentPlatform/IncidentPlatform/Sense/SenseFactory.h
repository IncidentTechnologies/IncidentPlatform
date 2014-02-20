// Factory for creating Sense Implementations

#pragma once

#include "SenseWin32.h"
#include "SenseDirectInput8.h"
#include "SenseWinMM.h"

// Sometimes the sense implementation may need the window Imp
#include "WindowImp.h"     

#include "list.h"

class SenseFactory
{
public:
    SenseFactory()
    {
        m_SenseType = SENSE_DEFAULT;
    }

    SenseFactory(SENSE_IMP_TYPE st)
    {
        m_SenseType = st;       
    }

    ~SenseFactory(){/*empty stub*/};

    // Make sense may have multiple sense implementations required by the user
    // Sense will manage these different devices as it sees fit so MakeSense is 
    // required to return a list of SenseImplmentations rather than just one
    virtual list<SenseImp*> *MakeSense(SENSE_IMP_TYPE SenseType, WindowImp *pWindowImp) const 
    {        
        list<SenseImp*> *pImpList = new list<SenseImp*>();
        
        int temp;
        
        temp = SenseType & SENSE_DEFAULT;

        if(temp == SENSE_DEFAULT)
        {
            //SenseImp* ptemp = new SenseImp();
            SenseImp *ptemp = NULL;
            pImpList->Append(ptemp);
        }

        temp = SenseType & SENSE_WIN32;

        if(temp == SENSE_WIN32)
        {
            SenseImp* ptemp = new SenseWin32(SenseType);
            pImpList->Append(ptemp);
        }

        temp = SenseType & SENSE_DIRECTINPUT8;

        if(SenseType & SENSE_DIRECTINPUT8 == SENSE_DIRECTINPUT8)
        {
            SenseImp *ptemp = new SenseDirectInput8(SenseType, pWindowImp);
            pImpList->Append(ptemp);
        }

        temp = SenseType & SENSE_WINMM;

        if(temp == SENSE_WINMM)
        {
            SenseImp *ptemp = new SenseWinMM(SenseType);
            pImpList->Append(ptemp);
        }

        return pImpList;
    }

    virtual list<SenseImp*> *MakeSense(WindowImp *pWindowImp) const
    {
        return this->MakeSense(m_SenseType, pWindowImp);
    }

private:
    SENSE_IMP_TYPE m_SenseType;
};