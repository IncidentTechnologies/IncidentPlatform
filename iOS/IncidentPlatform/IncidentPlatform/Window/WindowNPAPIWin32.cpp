#ifdef __WIN32__
#include "WindowNPAPIWin32.h"

// NPAPI Includes  (need to also add the SDK path to the includes)
#include "npapi.h"
#include "npupp.h"

#include "IncidentMain.h"

extern NPNetscapeFuncs NPNFuncs;

WindowNPAPIWin32::WindowNPAPIWin32( NPP instance, NPNetscapeFuncs *pNPNFuncs ) :
    m_Instance(instance),
    m_pStream(NULL),
    m_pScriptable(NULL),
    m_Initialized_f(false),
    m_hwnd(NULL),
    m_pNPNFuncs(pNPNFuncs)
{
    pNPNFuncs->getvalue(m_Instance, NPNVWindowNPObject, &sWindowObj);

    //*printf("Idans!\n");*/

    // We need to see if we can open a new console in here
    bool result = AllocConsole();

    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    if(hOutput == INVALID_HANDLE_VALUE)
    {
        assert(0);
    }
    else
    {
        WriteConsoleA(hOutput, "Debug Console:\n", strlen("Debug Console:\n"), NULL, NULL);
        printf("Idanz\n");
    }

    //RESULT r = IncidentMain();

}

WindowNPAPIWin32::~WindowNPAPIWin32()
{
    if(sWindowObj)
    {            
        m_pNPNFuncs->releaseobject(sWindowObj);
    }

    if(m_pScriptable)
    {
        m_pNPNFuncs->releaseobject(m_pScriptable);
    }

    sWindowObj = NULL;
}

NPBool WindowNPAPIWin32::Initialize( NPWindow *pWindow )
{
    if(pWindow == NULL)
    {
        return false;
    }

    m_hwnd = (HWND)pWindow->window;
    if(m_hwnd == NULL)
    {
        return false;
    }

    // Subclass the windows so that we may intercept
    // window messages and do our drawing into it instead
    s_pOldProc = SubclassWindow(m_hwnd, (WNDPROC)PluginWinProc);

    // Push the plug in into the user data so it can be
    // accessed from the Windows Process
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);

    m_pWindow = pWindow;

    m_Initialized_f = true;

    return true;
}

int16 WindowNPAPIWin32::HandleEvent( void *Event )
{
    NPEvent *pEv = (NPEvent *)Event;

    if(m_pWindow)
    {
        RECT box = {m_pWindow->y, m_pWindow->x,
            m_pWindow->y + m_pWindow->height, m_pWindow->x + m_pWindow->width};
        /*if(pEv->what == updateEvt)
        {
        ::TETextBox(m_pszString, strlen(m_pszString), &box, teJustCenter);
        } */         
    }

    return 0;
}

void WindowNPAPIWin32::ShowVersion()
{
    const char *ua = m_pNPNFuncs->uagent(m_Instance);
    strcpy(m_pszString, ua);

    InvalidateRect(m_hwnd, NULL, true);
    UpdateWindow(m_hwnd);

    if(m_pWindow)
    {
        NPRect r;

        r.top = (uint16)m_pWindow->y; 
        r.left = (uint16)m_pWindow->x;
        r.bottom = (uint16)(m_pWindow->y + m_pWindow->height);
        r.right = (uint16)(m_pWindow->x + m_pWindow->width);

        m_pNPNFuncs->invalidaterect(m_Instance, &r);
    }
}

void WindowNPAPIWin32::Clear()
{
    strcpy(m_pszString, "");

    InvalidateRect(m_hwnd, NULL, true);
    UpdateWindow(m_hwnd);
}

void WindowNPAPIWin32::Shut()
{
    // Subclass it back
    SubclassWindow(m_hwnd, s_pOldProc);
    m_hwnd = NULL;
    m_Initialized_f = false;
}

void WindowNPAPIWin32::GetVersion( char **ppVersion ) 
{
    const char* ua = m_pNPNFuncs->uagent(m_Instance);
    char* &rVersion = *ppVersion;

    rVersion = (char*)m_pNPNFuncs->memalloc(strlen(ua) + 1);

    if(rVersion)
    {
        strcpy(rVersion, ua);
    }

    return;
}


// NPAPI Windows Process
static LRESULT CALLBACK PluginWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            {            
                RECT rc;
                GetClientRect(hwnd, &rc);
                FrameRect(hdc, &rc, GetStockBrush(BLACK_BRUSH));
                WindowNPAPIWin32*pPlug = (WindowNPAPIWin32 *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                if(pPlug)
                {
                    if(pPlug->m_pszString[0] == 0)
                    {
                        strcpy(pPlug->m_pszString, "yay");
                    }
                    else
                    {
                        static int counter = 0;

                        sprintf(pPlug->m_pszString, "Idan Rulez:%d", counter);

                        counter++;                                   
                    }

                    DrawTextA(hdc, pPlug->m_pszString, strlen(pPlug->m_pszString), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

                }
            }
            EndPaint(hwnd, &ps);
        } break;

    case WM_LBUTTONDOWN:
        {
            RECT rc;
            GetClientRect(hwnd, &rc);

            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            if(rc.left < xPos && xPos < rc.right &&
                rc.top < yPos && yPos < rc.bottom)
            {            
                InvalidateRect(hwnd, NULL, true);
                UpdateWindow(hwnd);            
            }

            HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

            if(hOutput == INVALID_HANDLE_VALUE)
            {
                assert(0);
            }
            else
            {
                WriteConsoleA(hOutput, "You Clicked on me!\n", strlen("You Clicked on me!\n"), NULL, NULL);
            }

        } break;

    default:
        {
            /*empty stub*/
        } break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#endif // #ifdef __WIN32__