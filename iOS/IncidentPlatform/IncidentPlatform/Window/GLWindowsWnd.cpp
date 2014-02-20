#ifdef __WIN32__
#ifdef DIMUSE_OPENGL

#include "GLWindowsWnd.h"

const TCHAR kszGLWindowsWndClassName[] = TEXT("OpenGLWindowsWnd");

// This is used for WndProc rerouting
// There may be a better way to do this
GLWindowsWnd* g_GLWindowsWnd = NULL;

GLWindowsWnd::GLWindowsWnd() :
    m_fActive(TRUE),
    m_fFullscreen(FALSE),
    m_hrc(NULL),
    m_hdc(NULL),
    m_hwnd(NULL)
{ 
    // Prompt user for full screen
    if(MessageBox(NULL, TEXT("Would you like to run in fullscreen?"), TEXT("Full Screen"), 
    MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        m_fFullscreen = TRUE;
    }
    
    g_GLWindowsWnd = this;

    memset(&m_fKeys, 0, sizeof(m_fKeys));
    (void) CreateGLWindowsWnd(TEXT("Untitled"), 640, 480, 16, m_fFullscreen);
}

GLWindowsWnd::GLWindowsWnd(bool fFullscreen) :
    m_fActive(TRUE),
    m_fFullscreen(fFullscreen),
    m_hrc(NULL),
    m_hdc(NULL),
    m_hwnd(NULL)
{ 
    g_GLWindowsWnd = this;
    memset(&m_fKeys, 0, sizeof(m_fKeys));
    (void) CreateGLWindowsWnd(TEXT("Untitled"), 640, 480, 16, m_fFullscreen);
}

GLWindowsWnd::~GLWindowsWnd()
{
    // Deinitializes everything
    (void)KillWindow();
}

HRESULT GLWindowsWnd::Initialize()
{
    HRESULT hr = S_OK;
    
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    return hr;
}

HRESULT GLWindowsWnd::CreateGLWindowsWnd(TCHAR *pszTitle, int iWidth, int iHeight, int cBits, bool fFullscreen)
{
    HRESULT hr = S_OK;

    UINT iPixelFormat = {0};
    WNDCLASS wc = {0};
    
    // Window Styles
    DWORD dwExStyle;
    DWORD dwStyle;

    RECT rcWnd;

    // Create the PIXELFORMATDESCRIPTOR
    static PIXELFORMATDESCRIPTOR pfd = 
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        cBits,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        16,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    hr = CreateRect(0, 0, iWidth, iHeight, &rcWnd);
    CHR(hr);

    m_fFullscreen = fFullscreen;

    // Get instance for window
    m_hInstance = GetModuleHandle(NULL);

    // Register Window
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC) staticGLWindowsWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = kszGLWindowsWndClassName;
    CBR(RegisterClass(&wc));

    if(m_fFullscreen)
    {
        DEVMODE dmScreenSettings;

        // Clear Memory
        memset(&dmScreenSettings, 0, sizeof(DEVMODE));
        
        dmScreenSettings.dmSize = sizeof(DEVMODE);
        dmScreenSettings.dmPelsWidth = iWidth;
        dmScreenSettings.dmPelsHeight = iHeight;
        dmScreenSettings.dmBitsPerPel = cBits;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        CBR(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL);

        dwExStyle = WS_EX_APPWINDOW;
        dwStyle = WS_POPUP;
        ShowCursor(FALSE);
    }
    else
    {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        dwStyle = WS_OVERLAPPEDWINDOW;
    }

    // Adjust window to true requested size
    CBR(AdjustWindowRectEx(&rcWnd, dwStyle, FALSE, dwExStyle));

    m_hwnd = CreateWindowEx(dwExStyle, 
                            kszGLWindowsWndClassName,
                            pszTitle,
                            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
                            0, 0,
                            rcWnd.right - rcWnd.left,
                            rcWnd.bottom - rcWnd.top,
                            NULL,
                            NULL,
                            m_hInstance,
                            NULL );
    
    // Check to see window created correctly
    CBR(m_hwnd != NULL);

    m_hdc = GetDC(m_hwnd);
    CBR(m_hdc != NULL);

    iPixelFormat = ChoosePixelFormat(m_hdc, &pfd);
    CBR(iPixelFormat != 0);

    CBR(SetPixelFormat(m_hdc, iPixelFormat, &pfd));

    m_hrc = wglCreateContext(m_hdc);
    CBR(m_hrc != NULL);

    // Try to activate the rendering context
    CBR(wglMakeCurrent(m_hdc, m_hrc));

    // Show window and set up focus and perspective
    // Note: CBR crashes
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);

    hr = ReSizeGLWindowsWnd(iWidth, iHeight);
    CHR(hr);

    hr = Initialize();
    CHR(hr);

Error:
    return hr;
}   

HRESULT GLWindowsWnd::DrawScene()
{
    HRESULT hr = S_OK;

    // Clear screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(-1.5f, 0.0f, -6.0f);

    glBegin(GL_TRIANGLES);
        glVertex3f( 0.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f,-1.0f, 0.0f);
        glVertex3f( 1.0f,-1.0f, 0.0f);
    glEnd();


    return hr;
}

// Resize and Initialize the GL Window
HRESULT GLWindowsWnd::ReSizeGLWindowsWnd(UINT iWidth, UINT iHeight)
{
    HRESULT hr = S_OK;
    
    // Prevent divide by zero
    if(iHeight == 0)
    {
        iHeight = 1;
    }

    // reset current view port
    glViewport(0, 0, iWidth, iHeight);

    // Select and reset projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Calculate the aspect ratio of the window
    gluPerspective(45.0f, ((float)iWidth)/((float)iHeight), 0.1f, 100.0f);

    // Select and reset modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    return hr;
}

// Windows Functionality
int WINAPI GLWindowsWnd::WinMain(  
                    HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nCmdShow
                    )
{
    HRESULT hr = S_OK;
    MSG msg;
    BOOL done = FALSE;

    while(!done)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
            {
                done = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            if(GLWindowsWndMain->IsActive())
            {
                if(m_fKeys[VK_ESCAPE])
                {
                    done = TRUE;
                }
                else
                {
                    hr = DrawScene();
                    CHR(hr);

                    SwapBuffers(m_hdc);
                }
            }
        }
    }

Error:
    return (msg.wParam);
}

LRESULT CALLBACK GLWindowsWnd::staticGLWindowsWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    /*
    GLWindowsWnd *pParent;
    
    if(uMsg == WM_CREATE)
    {
        pParent = (GLWindowsWnd *)((LPCREATESTRUCT)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)pParent);
    }
    else
    {
        pParent = (GLWindowsWnd *)GetWindowLongPtr(hwnd, GWL_USERDATA);

        if(!pParent)
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    */
    if(g_GLWindowsWnd != NULL)
    {
        return g_GLWindowsWnd->GLWindowsWndProc(hwnd, uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT CALLBACK GLWindowsWnd::GLWindowsWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_ACTIVATE:
            {
                if(!HIWORD(wParam))
                {
                    m_fActive = TRUE;
                }
                else
                {
                    m_fActive = FALSE;
                }
                return 0;
            }

        case WM_SYSCOMMAND:
            {
                // Prevent system from entering powersave / screen saver
                switch(wParam)
                {
                    case SC_SCREENSAVE:
                    case SC_MONITORPOWER:
                        return 0;
                }
                break;
            }

        case WM_CLOSE:
            {
                // Send Quit message
                PostQuitMessage(0);
                return 0;
            }

        case WM_KEYDOWN:
            {
                m_fKeys[wParam] = TRUE;
                return 0;
            }

        case WM_KEYUP:
            {
                m_fKeys[wParam] = FALSE;
                return 0;
            }

        case WM_SIZE:
            {
                // loword = width, hiword = height
                ReSizeGLWindowsWnd(LOWORD(lParam), HIWORD(lParam));
                return 0;
            }
    }

    // Pass all unhandled messages to DefWindowProc
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HRESULT GLWindowsWnd::KillWindow()
{
    HRESULT hr = S_OK;

    if(m_fFullscreen)
    {
        // Switch back to desktop and show mouse pointer
        ChangeDisplaySettings(NULL, 0); 
        ShowCursor(TRUE);
    }

    if(m_hrc)
    {
        // Release DC and RC contexts
        CBR(wglMakeCurrent(NULL, NULL));
        
        // Delete rendering context
        CBR(wglDeleteContext(m_hrc));
        m_hrc = NULL;
    }

    if(m_hdc)
    {
        // Release the DC
        CBR(ReleaseDC(m_hwnd, m_hdc));
        m_hdc = NULL;
    }

    if(m_hwnd)
    {
        // Destroy Window
        CBR(DestroyWindow(m_hwnd));
        m_hwnd = NULL;
    }

    // Unregister Class
    CBR(UnregisterClass(kszGLWindowsWndClassName, m_hInstance));
    m_hInstance = NULL;

Error:
    return hr;
}

#endif // #ifdef __WIN32__
#endif // #ifdef DIMUSE_OPENGL