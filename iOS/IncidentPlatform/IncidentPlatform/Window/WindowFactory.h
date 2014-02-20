// Factory for creating window objects
// To properly create a window the client application will
// use this object to create a window with a proper implementation.

#pragma once

#include "WindowImp.h"

typedef enum Window_Types {
    WINDOW_DEFAULT,
    WINDOW_WIN32,
    WINDOW_X11,
    WINDOW_MAC
} WINDOW_TYPE;

class WindowFactory {
public:
    WindowFactory();
    WindowFactory(WINDOW_TYPE wt);
    ~WindowFactory();

    virtual WindowImp* MakeWindow(WINDOW_TYPE windowType) const;
    virtual WindowImp* MakeWindow() const;

private:
    WINDOW_TYPE m_windowType;
};