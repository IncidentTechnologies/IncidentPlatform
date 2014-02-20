// Generic Window Class
// Abstract class intended for general window purposes
// Abstracts implementation of the actual window or display area
// For all purposes window is used interchangably with display area.

#pragma once

#include "cmnPriv.h"
#include "WindowFactory.h"

class Window
{
public:
    Window() :
      m_pWindowImp(NULL)
    {/*empty stub*/};

    ~Window()
    { 
        // Kill The Window
        if(m_pWindowImp != NULL)
        {
            delete m_pWindowImp;
            m_pWindowImp = NULL;
        }
    };

    virtual RESULT BuildWindow(WindowFactory& factory) = 0;

    /* NOTE: Should Start Main really be defined as virtual?  */
    virtual RESULT StartMain(RESULT (*F[])(void*), int Functions_c) = 0;

    virtual int GetWinWidth() = 0;
    virtual int GetWinHeight() = 0;
    virtual int GetBufferWidth() = 0;
    virtual int GetBufferHeight() = 0;

private:

public:
    // Window Implementation
    WindowImp *m_pWindowImp;
};