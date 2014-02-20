#ifdef __WIN32__
#include "DX9Text.h"

RESULT DX9Text::InitializeFont()
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;

    CNRM(m_pd3device9, "DX9Text: Can't set up the font when m_pd3device9 not initialized");

    // In the case that the font is ALREADY initialized we need to destroy it (correctly)
    // and then reinitialize it
    if(m_dxfont != NULL)
    {
        m_dxfont->Release();        // no need to delete since directX will take care of this one?        
        m_dxfont = NULL;
    }

    // Create the font
    CHRM(D3DXCreateFont( m_pd3device9,                          // DX9 device
                         GetFontHeight(),                       // font height
                         GetFontWidth(),                        // default font width
                         (GetFontWeight() * 100),               // font weight
                         1,                                     // not using MipLevels
                         GetFontItalics(),                      // italics
                         DEFAULT_CHARSET,                       // character set
                         OUT_DEFAULT_PRECIS,                    // Output precision
                         DEFAULT_QUALITY,                       // Quality
                         DEFAULT_PITCH | FF_DONTCARE,           // default pitch and family
                         (LPCWSTR) GetFontName(),               // font type
                         &m_dxfont                              // target 
                       ), "DX9Text: D3DXCreateFont failed!");          

    if(m_dxfont == NULL)
    {
        printf("Could not create font %s\n", GetFontName());
    }

Error:
    return r;
}

RESULT DX9Text::Initialize( DimensionImp *pDimImp )
{
    HRESULT hr = S_OK;
    RESULT r = R_OK;

    LPDIRECT3DDEVICE9 pd3device9 = reinterpret_cast<DimensionImpDX9 *>(pDimImp)->GetDirectX9Device();
    CHM(pd3device9, "DX9Text: Could not get DirectX9 Device!\n");
    m_pd3device9 = pd3device9;

    CRM(InitializeFont(), "DX9Text: Failed to create the font");

Error:
    return r;
}

RESULT DX9Text::Render( DimensionImp* pDimImp )
{
    RESULT r = R_OK;
    LPDIRECT3DDEVICE9 pd3device9 = reinterpret_cast<DimensionImpDX9*>(pDimImp)->GetDirectX9Device();
    CNRM(pd3device9, "DX9Text: Could not get DX9 Device!");

    return Render(pd3device9);    

Error:
    return r;
}

// This function looks at the string and returns 
// a blit that would fit the string in it
blit DX9Text::GetBlit()
{
    RECT textbox;
    
    HRESULT hr = m_dxfont->DrawTextA( NULL,
                                      GetBuffer(),
                                      (INT) strlen(GetBuffer()),
                                      &textbox,
                                      DT_LEFT | DT_NOCLIP | DT_TOP | DT_CALCRECT,
                                      D3DCOLOR_ARGB(cbA(GetFontColor()), 
                                      cbR(GetFontColor()), 
                                      cbG(GetFontColor()), 
                                      cbB(GetFontColor()))
                                    );

    return blit(m_ptOrigin, textbox.bottom - textbox.top, textbox.right - textbox.left, BLIT_SCREEN);
}

RESULT DX9Text::Render( LPDIRECT3DDEVICE9 pd3device9 )
{
    RESULT r = R_OK;        
    HRESULT hr = S_OK;  
    static RECT textbox;        

    CPR(m_dxfont);
    CRM(Update(), "DX9Text: Failed to update!");

    SetRect(&textbox, pX(m_ptOrigin), pY(m_ptOrigin), 
                      pX(m_ptOrigin) + 1, pY(m_ptOrigin) + 1);

    hr = m_dxfont->DrawTextA( NULL,
                              GetBuffer(),
                              (INT) strlen(GetBuffer()),
                              &textbox,
                              DT_LEFT | DT_NOCLIP | DT_TOP,
                              D3DCOLOR_ARGB(cbA(GetFontColor()), 
                              cbR(GetFontColor()), 
                              cbG(GetFontColor()), 
                              cbB(GetFontColor()))
                            );

Error:
    return r;
}

RESULT DX9Text::Update()
{
    RESULT r = R_OK;

    CNRM(m_pd3device9, "DX9Text: m_pd3device9 has not been initialized");

    if(GetFont()->CheckDirtyClean())
    {
        CRM(InitializeFont(), "DX9Text: Failed to UPDATE the font");
    }

Error:
    return r;
}

DX9Text::~DX9Text()
{
    if(m_dxfont != NULL)
    {
        m_dxfont->Release();
        m_dxfont = NULL;
    }
}

DX9Text::DX9Text( point ptOrigin ) :
    text(ptOrigin),
m_dxfont(NULL)
{/*empty stub*/}

DX9Text::DX9Text( point ptOrigin, const char* c_str ) :
    text(ptOrigin, c_str),
m_dxfont(NULL)
{/*empty stub*/}

DX9Text::DX9Text( FlatObject *obj ) :
    text(obj->m_ptOrigin, static_cast<text*>(obj)->GetBuffer(), static_cast<text*>(obj)->GetFont()),
m_dxfont(NULL)
{/*empty stub*/}

#endif // #ifdef __WIN32__
