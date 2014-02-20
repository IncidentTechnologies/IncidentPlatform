#ifdef __WIN32__

#pragma once

#include "WindowImp.h"
#include "DX9Primitives.h"


// 256 Possible Keystrokes
#define KEYBOARD_RANGE 256

//  This is an object to create an directX window                                   //
//  Notice that this is being written August 2007 and that DirectX 10 has           //
//  recently been relesed.  This file will contain two different classes for each   //
//  version to allow a programmer to choose which one he wishes to use since many   //
//  video cards will not have correct support for DirectX 9.  However, these have   //
//  been developed for use with DirectX 10 in Windows Vista                         //

// NOTE:  Remember to add :
// C:\Program Files\Microsoft DirectX SDK (August 2008)\Include
// C:\Program Files\Microsoft DirectX SDK (August 2008)\Lib\x86
// to the VC++ directories for include / libraries

//#ifdef DIMUSE_DIRECTX9
    // Include and link all the DirectX 9 Stuff
    #include <d3d9.h>

    #pragma comment(lib, "d3d9.lib")
    #ifdef _DEBUG
        #pragma comment(lib,"d3dx9d.lib")
    #else
        #pragma comment(lib,"d3dx9.lib")
    #endif
/*#elif defined DIMUSE_DIRECTX10
    // Include and link all the DirectX 10 Stuff
    // This is tenative, I do not really know what these would look like yet.
    #include <d3d10.h>
    #include <d3dx10.h>

    #pragma comment(lib, "d3d10.lib")

    #ifdef _DEBUG
        #pragma comment(lib,"d3dx10d.lib")
    #else
        #pragma comment(lib,"d3dx10.lib")
    #endif
*/

class DX9WindowsWnd //: public WindowImp
{
public:
    // Constructor
    DX9WindowsWnd();
    DX9WindowsWnd(bool fFullscreen);

    // Destructor
    ~DX9WindowsWnd();

        // Windows Functionality
    int main();

    HRESULT initialize();

private:
    HRESULT createWnd(TCHAR *pszTitle, int iWidth, int iHeight);

    HRESULT render();

    HRESULT initializeDimension();
    
    HRESULT killWnd();

    void* getParam(int)
    {
        return NULL;
    }

    // Redirects to DXWndProc
    static LRESULT CALLBACK staticDX9WindowsWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT CALLBACK DX9WindowsWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    BOOL m_fKeys[KEYBOARD_RANGE];   // Keyboard input table

public:
    HWND m_hwnd;  

    // FPS Display Stuff
    FPS m_fps;
    
    HINSTANCE m_hInstance;
    
    LPDIRECT3D9 m_pd3d9;
    LPDIRECT3DDEVICE9 m_pd3device9;

private:
    bool m_fFullscreen;	// full screen window or not

    // Object Database
    // currently not a database but discrete objects

    // Fonts
    //Font* m_pfontArial;


};

#endif // #ifdef __WIN32__