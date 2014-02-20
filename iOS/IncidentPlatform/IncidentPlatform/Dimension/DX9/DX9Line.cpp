#ifdef __WIN32__
#include "DX9Line.h"

DX9Line::DX9Line( point ptOrigin = point(), point ptEnd = point(), float LineWidth = 0.0f ) :
    line(ptOrigin, ptEnd, LineWidth),
    m_pd39Buf(NULL),
    m_ppTextures(NULL),
    m_pVertexShader(NULL),
    m_pVertexConstants(NULL),
    m_pPixelConstants(NULL),
    m_pPixelShader(NULL),
    m_pTempBuffer(NULL)
{
    /*empty stub*/
}

DX9Line::DX9Line( FlatObject *obj ) :
    line(*(static_cast<line*>(obj))),
    m_pd39Buf(NULL),
    m_ppTextures(NULL),
    m_pVertexShader(NULL),
    m_pVertexConstants(NULL),
    m_pPixelConstants(NULL),
    m_pPixelShader(NULL),
    m_pTempBuffer(NULL)
{
    /*empty stub*/
}

DX9Line::~DX9Line()
{
    m_pd39Buf->Release();
}

RESULT DX9Line::Initialize( DimensionImp *pDimImp )
{
    HRESULT hr = S_OK;
    RESULT r = R_OK;

    LPDIRECT3DDEVICE9 pd3device9 = reinterpret_cast<DimensionImpDX9 *>(pDimImp)->GetDirectX9Device();
    CHM(pd3device9, "DX9Line: Could not get DirectX9 Device!\n");

    pd3device9->CreateVertexBuffer( (LINE_VERTEX_COUNT + 2) * sizeof(vertex),
                                    D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                                    CUSTOMFVF,
                                    D3DPOOL_DEFAULT,
                                    &m_pd39Buf,
                                    NULL );

    const D3DVERTEXELEMENT9 d3dvertexdecl[] = 
    {
        {0,     0,      D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0},
        {0,     12,     D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,    0},
        {0,     24,     D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TANGENT,   0},
        {0,     36,     D3DDECLTYPE_FLOAT4,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR,     0},
        {0,     52,     D3DDECLTYPE_FLOAT2,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0},
        {0,     60,     D3DDECLTYPE_FLOAT1,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_PSIZE,     0},
        D3DDECL_END()
    };

    CHRM(pd3device9->CreateVertexDeclaration(d3dvertexdecl, &m_pVertexDecl), "DX9Line: Could not create vertex declaration");

    // Render states will need to be set by a special component of the material manager!!!!
    if(pd3device9 != NULL)
    {        
        int TextureCount = GetTextureCount(TEXTURE_DIFFUSE);
        if(TextureCount > 0)
        {
            m_ppTextures = new IDirect3DTexture9*[TextureCount];
            for(int i = 0; i < TextureCount; i++)
            {
                char *TempPath = NULL;
                r = GetTexture(TEXTURE_DIFFUSE).GetTexture(i, TempPath);

                BSTR TexturePath;

                int lenA = lstrlenA(TempPath);
                int lenW;

                lenW = MultiByteToWideChar(CP_ACP, NULL, TempPath, -1, NULL, NULL);

                if(lenW == 0) 
                    printf("Failed to allocate BSTR!\n");

                TexturePath = SysAllocStringLen(0, lenW);
                MultiByteToWideChar(CP_ACP, NULL, TempPath, -1, TexturePath, lenW);               

                hr = D3DXCreateTextureFromFile(pd3device9, TexturePath, &m_ppTextures[i]);

                if(m_ppTextures[i] == NULL || FAILED(hr))
                {
                    printf("Failed to create texture %d\n", i);
                    m_ppTextures[i] = NULL;
                }   
                else
                {
                    printf("Added texture:%d : %s\n", i, TempPath);
                }
            }
        }
    }    
    // **

    // Set up the vertex shader (offset this to another object)
    char *pszVertexShaderProfile = (char*)(D3DXGetVertexShaderProfile(pd3device9));
    printf("Using Vertex Shader: %s\n", pszVertexShaderProfile);
    hr = D3DXCompileShaderFromFile(L"./shader/flat/vertex.vsh",      // shader file
        NULL,                        // Macros
        NULL,                        // includes
        "vs_main",                   // main funtion
        pszVertexShaderProfile,      // shader profile
        0,                           // flags
        &m_pTempBuffer,              // compiled operations
        NULL,                        // errors
        &m_pVertexConstants          // constants
        );
    CHRM(hr, "DX9Line::failed to create vertex shader");

    CHRM(pd3device9->CreateVertexShader((DWORD*)m_pTempBuffer->GetBufferPointer(), &m_pVertexShader), 
        "DX9Rectange::Could not create the vertex shader!");

    m_pTempBuffer->Release();
    // **

    ///*
    char *pszPixelShaderProfile = (char*)(D3DXGetPixelShaderProfile(pd3device9));
    printf("Using Pixel Shader: %s\n", pszPixelShaderProfile);

    // Set up the pixel shader (offset this to a different object)
    hr = D3DXCompileShaderFromFile(L"./shader/flat/pixel.psh",   // shader file
        NULL,                    // Macros
        NULL,                    // includes
        "ps_main",               // main funtion
        pszPixelShaderProfile,   // shader profile
        0,                       // flags
        &m_pTempBuffer,          // compiled operations
        NULL,                    // errors
        &m_pPixelConstants        // constants
        ); 
    CHRM(hr, "DX9Line::failed to create pixel shader");

    CHRM(pd3device9->CreatePixelShader((DWORD*)m_pTempBuffer->GetBufferPointer(), &m_pPixelShader),
        "DX9Rectange::Could not create the pixel shader!");

    m_pTempBuffer->Release();
    // **/


Error:
    return r;
}

RESULT DX9Line::Update()
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;
    void* pVoid;

    vertex CombinedVerts[LINE_VERTEX_COUNT + 2];

    D3DXMATRIX matTemp;

    D3DXMatrixIdentity(&m_matTransform);

    if( m_Scale != 1.0f )
    {   
        // Scale only if scale is not 1
        //D3DMatrixScale(&m_matTransform);
    }

    if( m_RotationX != 0.0f )
    {
        D3DXMatrixRotationX(&matTemp, m_RotationX);
        D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
    }

    if(m_RotationY != 0.0f)
    {
        D3DXMatrixRotationY(&matTemp, m_RotationY);
        D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
    }

    if(m_RotationZ != 0.0f)
    {
        D3DXMatrixRotationZ(&matTemp, m_RotationZ);
        D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
    }

    if( m_ptOrigin != point(0.0f, 0.0f, 0.0f))
    {
        // Translate only if not at origin anyways
        D3DXMatrixTranslation(&matTemp, m_ptOrigin.tuple[0], m_ptOrigin.tuple[1], m_ptOrigin.tuple[2]);
        D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
    }        

    memcpy(CombinedVerts, m_Vertices, sizeof(m_Vertices));
    CombinedVerts[4] = m_LineVertices[0];
    CombinedVerts[5] = m_LineVertices[1];

    // lock the buffer and load in the vertices's
    m_pd39Buf->Lock(0, 0, (void**)&pVoid, 0);
        memcpy(pVoid, CombinedVerts, sizeof(CombinedVerts));
    m_pd39Buf->Unlock();

Error:
    return r;
}

RESULT DX9Line::Render( DimensionImp* pDimImp )
{
    RESULT r = R_OK;

    LPDIRECT3DDEVICE9 pd3device9 = reinterpret_cast<DimensionImpDX9 *>(pDimImp)->GetDirectX9Device();

    if(pd3device9 != NULL)
        return Render(pd3device9);
    else
        return R_ERROR;

Error:
    return r;
}

RESULT DX9Line::Render( LPDIRECT3DDEVICE9 pd3device9 )
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;

    hr = Update();
    CHR(hr);


    //pd3device9->SetRenderState(D3DRS_LIGHTING, FALSE);
    //pd3device9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // Set World view Matrix
    //pd3device9->SetTransform(D3DTS_WORLD, &(m_matTransform));

    //m_pVertexConstants->SetMatrix(pd3device9, "WorldViewProj", &m_matTransform);  // pass this into vertex shader
    ///*
    D3DXMATRIXA16 matWorld, matView, matProj;
    pd3device9->GetTransform(D3DTS_WORLD, &matWorld);
    pd3device9->GetTransform(D3DTS_VIEW, &matView);
    pd3device9->GetTransform(D3DTS_PROJECTION, &matProj);

    D3DXMATRIXA16 matWorldViewProj = matWorld * matView * matProj;
    m_pVertexConstants->SetMatrix(pd3device9, "g_WorldViewProj", &matWorldViewProj);
    m_pVertexConstants->SetMatrix(pd3device9, "g_Transform", &m_matTransform);             

    // Do this statically for testing purposes
    m_pVertexConstants->SetFloatArray(pd3device9, "g_ViewPosition", point(0.0f, 5.0f, 0.0f).tuple, 3);
    m_pVertexConstants->SetFloatArray(pd3device9, "g_light0", point(2.0f, 5.0f, -2.0f).tuple, 3);

    // Vertex Decl
    pd3device9->SetVertexDeclaration(m_pVertexDecl);

    // Shaders
    pd3device9->SetVertexShader(m_pVertexShader);
    pd3device9->SetPixelShader(m_pPixelShader);

    pd3device9->SetStreamSource(0, m_pd39Buf, 0, sizeof(vertex));

    for(int i = 0; i < GetTextureCount(TEXTURE_DIFFUSE); i++)
    {
        if(m_ppTextures[i] != NULL)
        {
            pd3device9->SetTexture(i, m_ppTextures[i]);
        }
        else
        {
            pd3device9->SetTexture(i, NULL);
        }
    }

    pd3device9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    //pd3device9->DrawPrimitive(D3DPT_LINELIST, 4, 1);

    //pd3device9->SetRenderState(D3DRS_LIGHTING, TRUE);


Error:
    return r;
}

#endif // #ifdef __WIN32__
