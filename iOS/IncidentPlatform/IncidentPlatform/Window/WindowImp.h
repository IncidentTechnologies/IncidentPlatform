// Window Implementation Class
// Abstract interface for window implementation.  
// Window respresents a display area in this definition.

#pragma once

//#include "DX9Primitives.h"
//#include "Dimension.h"

//#include <windows.h>
#include "EHM.h"

using namespace dss;

typedef enum WindowParams
{
    WINDOW_FULLSCREEN,
    WINDOW_HANDLE,
    WINDOW_WIDTH,
    WINDOW_HEIGHT
} WINDOW_PARAM;

class WindowImp {
public:
    WindowImp(){ };
    WindowImp(bool fFullScreen){ };

    ~WindowImp(){ };
    
    virtual RESULT initialize() = 0;
    virtual RESULT HandleMessages() = 0;
    virtual RESULT createWnd(char *pszTitle, int width, int height) = 0;
    virtual RESULT killWnd() = 0;

    virtual void* getParam(WINDOW_PARAM param) = 0;

private:

};