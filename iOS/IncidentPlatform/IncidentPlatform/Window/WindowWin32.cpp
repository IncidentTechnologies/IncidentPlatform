#ifdef __WIN32__
#include "WindowWin32.h"

const TCHAR kszWindowWin32ClassName[] = TEXT("Win32 Window");
const TCHAR kszWindowWin32Title[] = TEXT("Windows32 Window");

WindowWin32::WindowWin32() :
    m_fFullscreen( false ),
    m_WinWidth(DEFAULT_WIDTH),
    m_WinHeight(DEFAULT_HEIGHT)
{
    //memset(&m_fKeys, 0, sizeof(m_fKeys));
    
    createWnd(const_cast<TCHAR *>(kszWindowWin32Title), m_WinWidth, m_WinHeight);

    initialize();
}

WindowWin32::WindowWin32(bool fFullscreen) :
    m_fFullscreen( fFullscreen ),
    m_WinWidth(DEFAULT_WIDTH),
    m_WinHeight(DEFAULT_HEIGHT)
{
    //memset(&m_fKeys, 0, sizeof(m_fKeys));
    
    createWnd(const_cast<TCHAR *>(kszWindowWin32Title), m_WinWidth, m_WinHeight);

    initialize();
}

WindowWin32::~WindowWin32()
{ 
    /* Empty */ 
}

HRESULT WindowWin32::createWnd(TCHAR *pszTitle, int iWidth, int iHeight)
{
    RESULT r = R_OK;    /*FIX*/
    
    HRESULT hr = S_OK;
    WNDCLASSEX wc;
    RECT rcWnd;

    hr = CreateRect(0, 0, iWidth, iHeight, &rcWnd);
    CHR(hr);

    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) staticWndProc;
    wc.hInstance = m_hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = kszWindowWin32ClassName;
    CBR(RegisterClassEx(&wc));

    if(m_fFullscreen)
    {
        m_hwnd = CreateWindowEx(NULL,                           // Extended Windows Style
                                kszWindowWin32ClassName,        // Class Name
                                pszTitle,                       // Window Name
                                WS_EX_TOPMOST | WS_POPUP,       // Window Style
                                0,                              // Initial X position
                                0,                              // Initial Y position
                                rcWnd.right - rcWnd.left,       // Width
                                rcWnd.bottom - rcWnd.top,       // Height
                                NULL,                           // Parent
                                NULL,                           // Menu
                                m_hInstance,                    // Handle
                                this                            // Pointer to a value passed to the window
                                );
    }
    else
    {
        m_hwnd = CreateWindowEx(NULL, 
                                kszWindowWin32ClassName,
                                pszTitle,
                                WS_OVERLAPPEDWINDOW,
                                450, 
                                450,
                                rcWnd.right - rcWnd.left,
                                rcWnd.bottom - rcWnd.top,
                                NULL,
                                NULL,
                                m_hInstance,
                                this );
    }

    ShowWindow(m_hwnd, SW_SHOW);

Error:
    return hr;
}

HRESULT WindowWin32::initialize()
{
    HRESULT hr = S_OK;
    
Error:
    return hr;
}

void* WindowWin32::getParam(WINDOW_PARAM wp) 
{
    switch(wp)
    {
        case WIN32_FULLSCREEN:      
        {
            return (void *)m_fFullscreen;
        } break;

        case WIN32_HANDLE:          
        {
            return m_hwnd;
        } break;

        case WIN32_WIDTH:
        {
            return (void*)&m_WinWidth;
        } break;

        case WIN32_HEIGHT:
        {
            return (void*)&m_WinHeight;
        } break;

        default:                    
        {
            return NULL;
        } break;
    }

    return NULL;
}

RESULT WindowWin32::HandleMessages()
{
    MSG msg;
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);        
    }

    return SUCCEED;
}


LRESULT CALLBACK WindowWin32::staticWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    
    WindowWin32 *pParent;
    
    if(uMsg == WM_NCCREATE)
    {
		SetWindowLong(hwnd, GWL_USERDATA, (long)((LPCREATESTRUCT(lParam))->lpCreateParams));
    }
    
	pParent = (WindowWin32 *)GetWindowLong(hwnd, GWL_USERDATA);
	
	if(pParent)
	{
		pParent->WndProc(hwnd, uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK WindowWin32::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {      

        case WM_SYSCOMMAND:
        {
            // Prevent system from entering powersave / screen saver
            switch(wParam)
            {
                case SC_SCREENSAVE:
                {
                    printf("Prevented Screen Save\n");
                    return 0;
                } break;

                case SC_MONITORPOWER:
                {
                    printf("Prevented monitor power down\n");
                    return 0;
                } break;

                default:
                {

                } break;
            }            
        } break;

        case WM_CLOSE:
        {
            SendMessage(hwnd, WM_DESTROY, 0, 0);
        } break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        } break;
    }

    // Pass all unhandled messages to DefWindowProc
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


HRESULT WindowWin32::killWnd()
{
    HRESULT hr = S_OK;

    if(m_hwnd)
    {
        // Destroy Window
        CBR(DestroyWindow(m_hwnd));
        m_hwnd = NULL;
    }

    // Unregister Class
    CBR(UnregisterClass(kszWindowWin32ClassName, m_hInstance));
    m_hInstance = NULL;

Error:
    return hr;
}

#endif // #ifdef __WIN32__