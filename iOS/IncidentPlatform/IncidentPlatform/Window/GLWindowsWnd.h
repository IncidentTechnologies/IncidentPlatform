#ifdef __WIN32__

#pragma once
#ifdef DIMUSE_OPENGL
#include "cmnPriv.h"

// This is an object to create an openGL window

#ifdef DIMUSE_OPENGL
    
    // Include and link all the openGL stuff if the user defines DIMUSE_OPENGL
    
    #pragma comment(lib, "opengl32.lib")
    #pragma comment(lib, "glu32.lib")
    #pragma comment(lib, "glaux.lib")

    #include <gl\gl.h>      // OpenGL32 Library
    #include <gl\glu.h>     // GLu32 library
    #include <gl\glaux.h>   // GLaux library

#endif

class GLWindowsWnd {
public:
    // Constructors
    GLWindowsWnd();
    GLWindowsWnd(bool fFullscreen);

    // Deconstructor
    ~GLWnd();

    BOOL IsActive() { return m_fActive; }

    HRESULT DrawScene();

private:
    HRESULT Initialize();
    HRESULT CreateGLWnd(TCHAR *pszTitle, int iWidth, int iHeight, int cBits, bool fFullscreen);
    HRESULT ReSizeGLWnd(UINT iWidth, UINT iHeight);

    // first called that then calls GLWndProc
    static LRESULT CALLBACK staticGLWindowsWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT CALLBACK GLWindowsWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Entry point for program
    int WINAPI WinMain( HINSTANCE hInstance,
                        HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine,
                        int nCmdShow);

    HRESULT KillWindow();

public:
    bool m_fKeys[KEYBOARD_RANGE];   // Keyboard input table

private:
    bool m_fActive;          // Window active flag
    bool m_fFullscreen;    

public:
    HGLRC       m_hrc;           // Permanent rendering context
    HDC         m_hdc;           // Private GDI device context
    HWND        m_hwnd;          // Window handle
    HINSTANCE   m_hInstance;            // Instance of application
};

#endif // #ifdef DIMUSE_OPENGL

#endif // #ifdef __WIN32__