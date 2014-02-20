#ifdef __WIN32__

#include "DX9WindowsWnd.h"
//#include "cmnPriv.h"

//#ifdef DIMUSE_DIRECTX9
    const TCHAR kszDX9WindowsWndClassName[] = TEXT("DirecrX9Wnd");
/*
#elif defined DIMUSE_DIRECTX10
    const TCHAR kszDX10WndClassName[] = TEXT("DirecrX10Wnd");
*/

//DX9WindowsWnd* g_pDX9WindowsWnd = NULL;

// Testing grounds
   
// *******************************
DX9Triangle* g_pMyTri = NULL;
DX9Volume*  g_pMyVol = NULL;
DX9Camera* g_pMyCam = NULL;

float g_fSphereX = 0.0f, 
      g_fSphereY = 0, 
      g_fSphereZ = 0.0f;

float g_fRotationIndex = 0.0f;
// *******************************


DX9WindowsWnd::DX9WindowsWnd() :
    m_fFullscreen( false )
    //m_pfontArial(NULL)
{
    //g_pDX9WindowsWnd = this;
    //m_szFPSText[0] = '\0';

    memset(&m_fKeys, 0, sizeof(m_fKeys));
    
    // Note:  Must create the window handle before calling initialize!
    createWnd(TEXT("DirecrX9Wnd"), DEFAULT_WIDTH, DEFAULT_HEIGHT);
    initialize();
}

DX9WindowsWnd::DX9WindowsWnd(bool fFullscreen) :
    m_fFullscreen( fFullscreen )
    //m_pfontArial(NULL)
{
    //g_pDX9WindowsWnd = this;
    //m_szFPSText[0] = '\0';

    memset(&m_fKeys, 0, sizeof(m_fKeys));
    
    // Note:  Must create the window handle before calling initialize!
    createWnd(TEXT("DirecrX9Wnd"), DEFAULT_WIDTH, DEFAULT_HEIGHT);
    initialize();
}

DX9WindowsWnd::~DX9WindowsWnd(){ /* Empty */ }

HRESULT DX9WindowsWnd::createWnd(TCHAR *pszTitle, int iWidth, int iHeight)
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
    wc.lpfnWndProc = (WNDPROC) staticDX9WindowsWndProc;
    wc.hInstance = m_hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = kszDX9WindowsWndClassName;
    CBR(RegisterClassEx(&wc));

    if(m_fFullscreen)
    {
        m_hwnd = CreateWindowEx(NULL, 
                                kszDX9WindowsWndClassName,
                                pszTitle,
                                WS_EX_TOPMOST | WS_POPUP,
                                0, 0,
                                rcWnd.right - rcWnd.left,
                                rcWnd.bottom - rcWnd.top,
                                NULL,
                                NULL,
                                m_hInstance,
                                this );
    }
    else
    {
        m_hwnd = CreateWindowEx(NULL, 
                                kszDX9WindowsWndClassName,
                                pszTitle,
                                WS_OVERLAPPEDWINDOW,
                                0, 0,
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

// this is the function that sets up the lights and materials

void init_light(LPDIRECT3DDEVICE9 pd3device9)
{
    D3DLIGHT9 light;    // create the light struct
    D3DMATERIAL9 material;    // create the material struct

    ZeroMemory(&light, sizeof(light));    // clear out the struct for use
    light.Type = D3DLIGHT_POINT;    // make the light type 'directional light'
    light.Diffuse.r = 0.5f;    // .5 red
    light.Diffuse.g = 0.5f;    // .5 green
    light.Diffuse.b = 0.5f;    // .5 blue
    light.Diffuse.a = 1.0f;    // full alpha (we'll get to that soon)

    light.Range = 100.f;
    light.Attenuation0 = 0.0f;      // constant attenuation
    light.Attenuation1 = 0.125f;    // inverse attenuation
    light.Attenuation2 = 0.0f;      // square inverse attenuation

    D3DVECTOR vecDirection = {3.0f, 5.0f, 3.0f};    // the direction of the light
    light.Position = vecDirection;    // set the direction

    pd3device9->SetLight(0, &light);    // send the light struct properties to light #0
    pd3device9->LightEnable(0, TRUE);    // turn on light #0

    ZeroMemory(&material, sizeof(D3DMATERIAL9));    // clear out the struct for use
    material.Diffuse.r = material.Ambient.r = 1.0f;    // set the material to full red
    material.Diffuse.g = material.Ambient.g = 1.0f;    // set the material to full green
    material.Diffuse.b = material.Ambient.b = 1.0f;    // set the material to full blue
    material.Diffuse.a = material.Ambient.a = 1.0f;    // set the material to full alpha

    pd3device9->SetMaterial(&material);    // set the globably-used material to &material

    return;
}



HRESULT DX9WindowsWnd::initialize()
{
    RESULT r = R_OK;    /*FIX*/
    
    HRESULT hr = S_OK;
    D3DPRESENT_PARAMETERS d3dpp;

    // create the Direct3D interface
    if((m_pd3d9 = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    {
        hr = E_FAIL;
        CHR(hr);
    }

    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = (!m_fFullscreen);
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = m_hwnd;

    // full screen stuff?
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;    // set the back buffer format to 32-bit
    d3dpp.BackBufferWidth = DEFAULT_WIDTH;    // set the width of the buffer
    d3dpp.BackBufferHeight = DEFAULT_HEIGHT;    // set the height of the buffer

    // Z Buffer
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    m_pd3d9->CreateDevice(	D3DADAPTER_DEFAULT,
                            D3DDEVTYPE_HAL,
                            m_hwnd,
                            D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                            &d3dpp,
                            &m_pd3device9);

    init_light(m_pd3device9);    // call the function to initialize the light and material

    // Z Buffer
    m_pd3device9->SetRenderState(D3DRS_ZENABLE, TRUE);

    // Turn on lighting
    m_pd3device9->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pd3device9->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(25, 25, 25));

    m_pd3device9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


Error:
    return hr;
}

HRESULT DX9WindowsWnd::render()
{
    RESULT r = R_OK;    /*FIX*/
    
    HRESULT hr = S_OK;
    
    //DX9Sphere sph(point(g_fSphereX, g_fSphereY, g_fSphereZ), 0.5f);
    DX9Sphere sph(point(g_fSphereX, g_fSphereY, g_fSphereZ), 0.5f, 16, 16);
    sph.DX9Init(m_pd3device9);
    sph.DX9Update();

    m_pd3device9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    // Clear the zbuffer
    m_pd3device9->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    // Update the camera
    //hr = g_pMyCam->DX9Update(m_pd3device9);
    //CHR(hr);

    g_pMyVol->Nuke(point(g_pMyCam->GetOrigin().tuple[DIM_X], 0, 0));
    g_pMyVol->Nuke(sph);

    m_pd3device9->BeginScene();
        
        hr = g_pMyVol->Render(m_pd3device9);
        CHR(hr);

        sph.Render(m_pd3device9);
        
        // FPS Update and Display
        m_fps.update();
        //hr = m_pfontArial->PrintXY(DEFAULT_FPS_X, DEFAULT_FPS_Y, m_fps.m_szFPSText);
        CHR(hr);

    m_pd3device9->EndScene();

    // Display the created frame
    m_pd3device9->Present(NULL, NULL, NULL, NULL);

Error:
    return hr;
}

HRESULT DX9WindowsWnd::initializeDimension()
{
    RESULT r = R_OK;    /*FIX*/
    
    HRESULT hr = S_OK;

    // Create a font here
    //m_pfontArial = new Font(m_pd3device9, "Arial");
    //CBR(m_pfontArial != NULL);

    g_pMyCam = new DX9Camera
        (
            point(5.0f, 5.0f, 5.0f),
            point(0.0f, 0.0f, 0.0f),
            vector(0.0f, 1.0f, 0.0f)
        );

    g_pMyVol = new DX9Volume(point(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, 1.0f);
    hr = g_pMyVol->DX9Init(m_pd3device9);
    CHR(hr);

Error:
    return hr;
}

// Windows Functionality
// *************************************************
int DX9WindowsWnd::main()
{
    RESULT r = R_OK;    /*FIX*/
    
    HRESULT hr = S_OK;
    MSG msg;
    bool fDone = false;

    // Load in all the graphics etc.
    hr = initializeDimension();
    CHR(hr);

    while(!fDone)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                fDone = true;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        { 
            if(m_fKeys[VK_ESCAPE])
            {
                fDone = TRUE;
            }
            
            // Move Camera            
            if(m_fKeys['a'] || m_fKeys['A'])
            {
                g_pMyCam->MoveSideways(0.1f);
            }
            if(m_fKeys['d'] || m_fKeys['D'])
            {
                g_pMyCam->MoveSideways(-0.1f);
            }
            if(m_fKeys['w'] || m_fKeys['W'])
            {
                g_pMyCam->MoveForward(0.1f);
            }
            if(m_fKeys['s'] || m_fKeys['S'])
            {
                g_pMyCam->MoveForward(-0.1f);
            }

            // Move Sphere
            // DEBUGGIGN!
            if(m_fKeys['h'] || m_fKeys['H'])
            {
                g_fSphereX += .01;
                printf("sph: x: %f y: %f z: %f\n", g_fSphereX, g_fSphereY, g_fSphereZ);
            }
            if(m_fKeys['k'] || m_fKeys['K'])
            {
                g_fSphereX -= .01;
                printf("sph: x: %f y: %f z: %f\n", g_fSphereX, g_fSphereY, g_fSphereZ);
            }
            if(m_fKeys['u'] || m_fKeys['U'])
            {
                g_fSphereZ -= .01;
                printf("sph: x: %f y: %f z: %f\n", g_fSphereX, g_fSphereY, g_fSphereZ);
            }
            if(m_fKeys['j'] || m_fKeys['J'])
            {
                g_fSphereZ += .01;
                printf("sph: x: %f y: %f z: %f\n", g_fSphereX, g_fSphereY, g_fSphereZ);
            }
            if(m_fKeys['y'] || m_fKeys['Y'])
            {
                g_fSphereY += .01;
                printf("sph: x: %f y: %f z: %f\n", g_fSphereX, g_fSphereY, g_fSphereZ);
            }
            if(m_fKeys['g'] || m_fKeys['G'])
            {
                g_fSphereY -= .01;
                printf("sph: x: %f y: %f z: %f\n", g_fSphereX, g_fSphereY, g_fSphereZ);
            }

            
            // Rotate
            if(m_fKeys[VK_LEFT])
            {
                g_pMyCam->RotateY(-0.0175f);
            }
            if(m_fKeys[VK_RIGHT])
            {
                g_pMyCam->RotateY(0.0175f);
            }
            if(m_fKeys[VK_UP])
            {
                g_pMyCam->RotateX(0.0175f);
            }
            if(m_fKeys[VK_DOWN])
            {
                g_pMyCam->RotateX(-0.0175f);
            }            

            render();
        }
    }

Error:
    killWnd();
    return (int)(msg.wParam);
}



LRESULT CALLBACK DX9WindowsWnd::staticDX9WindowsWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    
    DX9WindowsWnd *pParent;
    
    if(uMsg == WM_NCCREATE)
    {
		SetWindowLong(hwnd, GWL_USERDATA, (long)((LPCREATESTRUCT(lParam))->lpCreateParams));
    }
    
	pParent = (DX9WindowsWnd *)GetWindowLong(hwnd, GWL_USERDATA);
	
	if(pParent)
	{
		pParent->DX9WindowsWndProc(hwnd, uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK DX9WindowsWnd::DX9WindowsWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {      

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
    
        case WM_CLOSE:
            {
                PostQuitMessage(0);
                return 0;
            } break;
    }

    // Pass all unhandled messages to DefWindowProc
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


HRESULT DX9WindowsWnd::killWnd()
{
    HRESULT hr = S_OK;

    m_pd3d9->Release();
    m_pd3device9->Release();

    if(m_hwnd)
    {
        // Destroy Window
        CBR(DestroyWindow(m_hwnd));
        m_hwnd = NULL;
    }

    // Unregister Class
    CBR(UnregisterClass(kszDX9WindowsWndClassName, m_hInstance));
    m_hInstance = NULL;

Error:
    m_fps.FPSDiag();
    return hr;
}

#endif // #ifdef __WIN32__
