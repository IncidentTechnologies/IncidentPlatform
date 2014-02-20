#pragma once

// The dirty class is a very simple syncronization class which acts very much like
// a semaphore.  Effectively much like a dirty bit that is thread safe.
// On initialization the dirty bit semaphore is set to 1, when the dirty bit 
// is being checked or accessed the semaphore is checked to ensure that it is 1
// if not the thread waits until it gets to be 1 at which point it does the magic.
// When the magic is over the semaphore is incremented (or set to 1)

#include "RESULT.h"

class dirty
{
public:
    dirty() :
        m_s(1),
        m_dirty(false)
    {/*emptystub*/}

    RESULT SetDirty()
    {
        while(m_s <= 0)
        {/*wait until semaphore is greater than 0*/}
        m_s--;

        // If not dirty then set dirty and return a success
        if(!m_dirty)
        {
            m_dirty = true;
            m_s++;
            return R_DIRTY_SUCCESS;
        }
        else
        {
            m_s++;
            return R_DIRTY_ALREADY_DIRTY;
        }
    }

    RESULT SetClean()
    {
        while(m_s <= 0)
        {/*wait until semaphore is greater than 0*/}
        m_s--;

        // If not dirty then set dirty and return a success
        if(m_dirty)
        {
            m_dirty = false;
            m_s++;
            return R_DIRTY_SUCCESS;
        }
        else
        {
            m_s++;
            return R_DIRTY_ALREADY_CLEAN;
        }
    }

    // Check dirty will check the status of the dirty bit
    // but will wait until someone is done with it
    bool CheckDirty()
    {
        bool TempDirty;        
        while(m_s <= 0)
        {/*wait until everyone is done with the semaphore*/}
        m_s--;
        TempDirty = m_dirty;
        m_s++;
        return TempDirty;
    }

    // This will check the dirty bit as before but will also
    // clean the dirty bit if it is dirty
    bool CheckDirtyClean()
    {
        bool TempDirty;        
        while(m_s <= 0)
        {/*wait until everyone is done with the semaphore*/}
        m_s--;
        TempDirty = m_dirty;
        m_dirty = false;
        m_s++;
        return TempDirty;
    }

private:
    bool m_dirty;
    int m_s;
};