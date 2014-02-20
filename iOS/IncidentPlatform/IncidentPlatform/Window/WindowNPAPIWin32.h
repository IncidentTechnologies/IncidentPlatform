#ifdef __WIN32__
#pragma once

#include "WindowImp.h"

#include <windows.h>
#include <windowsx.h>

// NPAPI Includes  (need to also add the SDK path to the includes)
#include "npapi.h"
#include "npupp.h"

#include "resource.h"

// Exports
NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* pFuncs);
NPError WINAPI NP_Initialize(NPNetscapeFuncs *pFuncs);
NPError OSCALL NP_Shutdown();

// Here for now, will move it soon
// ---------------------------------------------------------------------------------------
static NPObject *sWindowObj;

// Windows Process
static WNDPROC s_pOldProc = NULL;
static LRESULT CALLBACK PluginWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
// ---------------------------------------------------------------------------------------

class WindowNPAPIWin32
  // public WindowImp
{
public:
    // Constructor and Destructor
    WindowNPAPIWin32(NPP instance, NPNetscapeFuncs *pNPNFuncs);
    ~WindowNPAPIWin32();

    // Plugin Functionality Called By Netscape
    NPBool Initialize(NPWindow *pWindow);
    void Shut();
    int16 HandleEvent(void *Event);
    void ShowVersion();
    void Clear();
    void GetVersion(char **ppVersion);
    NPObject *GetScriptableObject();

    bool IsInitialized(){ return m_Initialized_f; }

private:
    NPP m_Instance;                 // Netscape Plugin Instance
    HWND m_hwnd;                    // Window Handle
    bool m_Initialized_f;           // Initialization Flag

    NPWindow *m_pWindow;            // Netscape Window
    NPStream *m_pStream;            // Netscape Stream    
    NPObject *m_pScriptable;        // Scriptable object (used for java script, not used yet)
    NPNetscapeFuncs *m_pNPNFuncs;   // Function Table passed by Netscape API

public:
    char m_pszString[MAX_PATH];
};

#endif // #ifdef __WIN32__