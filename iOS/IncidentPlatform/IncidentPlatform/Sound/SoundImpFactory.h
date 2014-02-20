#pragma once

// Sound Implementation Factory will create a 
// Sound Implementation according to the specific implementation chosen for the said
// factory.  The factory is passed to an arbitrary SoundObj and will return a
// SoundImp object.  The interfaces defined between the two will allow the 
// correct interaction

#include "SoundImpDirectSound.h"

typedef enum SoundImpType
{
    SOUND_IMP_DEFAULT,
    SOUND_IMP_DIRECTSOUND
} SOUND_IMP_TYPE;


class SoundImpFactory
{
public:
    SoundImpFactory()
    {
        m_SoundImpType = SOUND_IMP_DEFAULT;
    }

    SoundImpFactory(SOUND_IMP_TYPE SoundImpType)
    {
        m_SoundImpType = SoundImpType;
    }

    ~SoundImpFactory()
    {
        /*emptystub*/
    }

    virtual SoundImp* MakeSoundImp(SOUND_IMP_TYPE SoundImpType) const 
    {
        switch(SoundImpType)
        {
            case SOUND_IMP_DIRECTSOUND: 
            {
                return new SoundImpDirectSound();
            } break;            

            default:    
            {
                return NULL;
            } break;
        }                
    }

    virtual SoundImp* MakeSoundImp() const
    {
        return this->MakeSoundImp(m_SoundImpType);
    }
    
private:
    SOUND_IMP_TYPE m_SoundImpType;
};
