#ifdef __WIN32__
#pragma once

#include "WindowImp.h"
#include "cmnPriv.h"

typedef int WIN32_PARAM;

enum Win32_Params
{
    WIN32_FULLSCREEN = WINDOW_FULLSCREEN,
    WIN32_HANDLE = WINDOW_HANDLE,
    WIN32_WIDTH = WINDOW_WIDTH,
    WIN32_HEIGHT = WINDOW_HEIGHT
};

class WindowWin32 :
    public WindowImp
{
public:
    // Constructor
    WindowWin32();
    WindowWin32(bool fFullscreen);

    // Destructor
    ~WindowWin32();

    HRESULT initialize();

    RESULT HandleMessages();

    HWND GetHwnd(){ return m_hwnd; }

private:
    HRESULT createWnd(TCHAR *pszTitle, int iWidth, int iHeight);
    
    HRESULT killWnd();

    void* getParam(WINDOW_PARAM wp);    

    // Redirects to DXWndProc
    static LRESULT CALLBACK staticWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hwnd;
    HINSTANCE m_hInstance;
    bool m_fFullscreen;	// full screen window or not

    int m_WinWidth;
    int m_WinHeight;
                 
};

#endif // #ifdef __WIN32__