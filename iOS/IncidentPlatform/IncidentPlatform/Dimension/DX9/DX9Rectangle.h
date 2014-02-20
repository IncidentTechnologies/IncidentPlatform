#ifdef __WIN32__
// DirectX9 Rectangle that inherits from the primitive type
// rectangle and implements the DX9DimObject interface

#pragma once

struct CUSTOMVERTEX {FLOAT X, Y, Z; FLOAT NX, NY, NZ; DWORD COLOR; FLOAT U, V;};
#define CUSTOMFVF_TEMP (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

class DX9Rectangle :
    public rectangle,
    public DX9DimObject
{
public:
    DX9Rectangle() :
      rectangle(),
      m_pd39Buf(NULL),
      m_ppTextures(NULL),
      m_pVertexShader(NULL),
      m_pVertexConstants(NULL),
      m_pPixelConstants(NULL),
      m_pPixelShader(NULL),
      m_pTempBuffer(NULL)

    {

    }

    DX9Rectangle(point origin, float height, float width) :
      rectangle(origin, height, width),
      m_pd39Buf(NULL),
      m_ppTextures(NULL),
      m_pVertexShader(NULL),
      m_pVertexConstants(NULL),
      m_pPixelConstants(NULL),
      m_pPixelShader(NULL),
      m_pTempBuffer(NULL)
    {

    }

    DX9Rectangle(DimObject *obj) :
      rectangle(static_cast<rectangle*>(obj)->GetOrigin(), 
                static_cast<rectangle*>(obj)->GetHeight(),
                static_cast<rectangle*>(obj)->GetWidth()),
      m_pd39Buf(NULL),
      m_ppTextures(NULL),
      m_pVertexShader(NULL),
      m_pVertexConstants(NULL),
      m_pPixelConstants(NULL),
      m_pPixelShader(NULL),
      m_pTempBuffer(NULL)
    {
    
    }

    ~DX9Rectangle()
    {
        m_pd39Buf->Release();                
    }

public:
    HRESULT DX9Init(LPDIRECT3DDEVICE9 pd3device9)
    {
        HRESULT hr = S_OK;
        RESULT r = R_OK;

        pd3device9->CreateVertexBuffer
        (
            RECTANGLE_VERTEX_COUNT * sizeof(vertex),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            CUSTOMFVF,
            D3DPOOL_DEFAULT,
            &m_pd39Buf,
            NULL
        );
      
        const D3DVERTEXELEMENT9 d3dvertexdecl[] = 
        {
            {0,     0,      D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0},
            {0,     12,     D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,    0},
            {0,     24,     D3DDECLTYPE_D3DCOLOR,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR,     0},
            {0,     28,     D3DDECLTYPE_FLOAT2,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0},
            D3DDECL_END()
        };

        hr = pd3device9->CreateVertexDeclaration(d3dvertexdecl, &m_pVertexDecl); 
        if(FAILED(hr))
        {
            printf("dx9rectangle::Could not create vertex declaration\n");
        }
        // **

        // Texture Loading (maybe do this in DXDimObject?
        if(pd3device9 != NULL)
        {        
            // Only doing diffuse textures for now!
            if(GetTextureCount(TEXTURE_DIFFUSE) > 0)
            {
                m_ppTextures = new IDirect3DTexture9*[GetTextureCount(TEXTURE_DIFFUSE)];
                for(int i = 0; i < GetTextureCount(TEXTURE_DIFFUSE); i++)
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
        hr = D3DXCompileShaderFromFile(L"./shader/bump/vertex.vsh",      // shader file
                                       NULL,                        // Macros
                                       NULL,                        // includes
                                       "vs_main",                   // main funtion
                                       pszVertexShaderProfile,      // shader profile
                                       0,                           // flags
                                       &m_pTempBuffer,              // compiled operations
                                       NULL,                        // errors
                                       &m_pVertexConstants          // constants
                                       );
        CHRM(hr, "DX9Rectangle::failed to create vertex shader");

        CHRM(pd3device9->CreateVertexShader((DWORD*)m_pTempBuffer->GetBufferPointer(), &m_pVertexShader), 
             "DX9Rectange::Could not create the vertex shader!");

        m_pTempBuffer->Release();
        // **

        ///*
        char *pszPixelShaderProfile = (char*)(D3DXGetPixelShaderProfile(pd3device9));
        printf("Using Pixel Shader: %s\n", pszPixelShaderProfile);

        // Set up the pixel shader (offset this to a different object)
        hr = D3DXCompileShaderFromFile(L"./shader/bump/pixel.psh",   // shader file
                                       NULL,                    // Macros
                                       NULL,                    // includes
                                       "ps_main",               // main funtion
                                       pszPixelShaderProfile,   // shader profile
                                       0,                       // flags
                                       &m_pTempBuffer,          // compiled operations
                                       NULL,                    // errors
                                       &m_pPixelConstants        // constants
                                       ); 
        CHRM(hr, "DX9Rectangle::failed to create pixel shader");

        CHRM(pd3device9->CreatePixelShader((DWORD*)m_pTempBuffer->GetBufferPointer(), &m_pPixelShader),
             "DX9Rectange::Could not create the pixel shader!");

        m_pTempBuffer->Release();
        // **/
        
        
Error:
        return hr;
    }

    HRESULT DX9Update()
    {
        HRESULT hr = S_OK;
        void* pVoid;
        D3DXMATRIX matTemp;

        D3DXMatrixIdentity(&m_matTransform);

        if( m_fScale != 1.0f )
        {   
            // Scale only if scale is not 1
            //D3DMatrixScale(&m_matTransform);
        }

        if( m_fRotationX != 0.0f )
        {
            D3DXMatrixRotationX(&matTemp, m_fRotationX);
            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }
        if(m_fRotationY != 0.0f)
        {
            D3DXMatrixRotationY(&matTemp, m_fRotationY);
            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }
        if(m_fRotationZ != 0.0f )
        {
            D3DXMatrixRotationZ(&matTemp, m_fRotationZ);
            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }
        if( m_ptOrigin != point(0.0f, 0.0f, 0.0f))
        {
            // Translate only if not at origin anyways
            D3DXMatrixTranslation(&matTemp, m_ptOrigin.tuple[0], m_ptOrigin.tuple[1], m_ptOrigin.tuple[2]);
            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }        

        // lock the buffer and load in the vertices's
        m_pd39Buf->Lock(0, 0, (void**)&pVoid, 0);
            memcpy(pVoid, m_Vertices, sizeof(m_Vertices));
        m_pd39Buf->Unlock();
        

        return hr;
    }

    RESULT Render(DimensionImpDX9* pDimImpDX9)
    {
        return R_NOT_IMPLEMENTED;
    }

    RESULT Render(LPDIRECT3DDEVICE9 pd3device9)
    {
        RESULT r = R_OK;
        HRESULT hr = S_OK;

        /*
        D3DXMATRIX matIdentity;
        D3DXMATRIX matOrtho;
        D3DXMATRIX matPerspective;
        D3DXMATRIX matView;

        D3DXMatrixIdentity(&matIdentity);

        D3DXMatrixPerspectiveFovLH( &matPerspective,                            // Projection Matrix
                                    D3DXToRadian(45),                               // the horizontal field of view
                                    (FLOAT)DEFAULT_WIDTH / (FLOAT)DEFAULT_HEIGHT,   // aspect ratio
                                    1.0f,                                           // the near view-plane
                                    100.0f                                          // the far view-plane
                                  );    

        D3DXMatrixOrthoOffCenterLH( &matOrtho,
                                    -320.0f,
                                    320.0f,
                                    -240.0f,
                                    240.0f,
                                    -50.0f,
                                    50.0f
                                  );

        D3DXMatrixOrthoLH( &matOrtho,
                           320.0f,
                           240.0f,
                           0.0f,
                           100.0f
                         ); 

        D3DXMatrixLookAtLH( &matView,
                            &D3DXVECTOR3(0.0f, 0.0f, 1.0f),                 // Camera Origin
                            &D3DXVECTOR3(0.0f, 0.0f, 0.0f),                 // Look At Position
                            &D3DXVECTOR3(0.0f, 1.0f, 0.0f)                  // Up Direction
                          );        
                          */

        hr = DX9Update();
        CHR(hr);

        //pd3device9->SetRenderState(D3DRS_LIGHTING, FALSE);
        //pd3device9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);



        /* This is how we would make it a blit
        pd3device9->SetTransform(D3DTS_WORLD, &matIdentity);
        pd3device9->SetTransform(D3DTS_VIEW, &matView);       
        pd3device9->SetTransform(D3DTS_PROJECTION, &matPerspective);        
        pd3device9->SetTransform(D3DTS_PROJECTION, &matOrtho);      
        */



        // Set World view Matrix
        pd3device9->SetTransform(D3DTS_WORLD, &(m_matTransform));

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
        
        static float mult = 0.0f;
        mult += .025;

        m_pPixelConstants->SetFloat(pd3device9, "g_Multiplier", abs(sin(mult)));
        
        /*
        // Complex Way to Do this!        
        D3DXHANDLE hTex0 = m_pPixelConstants->GetConstantByName(NULL, "Tex1");
        if(hTex0)
        {
            D3DXCONSTANT_DESC constDesc;
            unsigned int count;
            m_pPixelConstants->GetConstantDesc(hTex0, &constDesc, &count);
            if(constDesc.RegisterSet == D3DXRS_SAMPLER)
            {
                //pd3device9->SetTexture( ... );

                //pd3device9->SetSamplerState( ... );
            }
        }
        */
        

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
                //pd3device9->SetSamplerState(i, ..., ...);
            }
            else
            {
                pd3device9->SetTexture(i, NULL);
            }
        }

        pd3device9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

        //pd3device9->SetRenderState(D3DRS_LIGHTING, TRUE);


Error:
        return r;
    }

public:
    IDirect3DVertexBuffer9* m_pd39Buf;
    IDirect3DVertexDeclaration9 *m_pVertexDecl;
    
    // SHADERS
    IDirect3DVertexShader9 *m_pVertexShader;
    ID3DXConstantTable *m_pVertexConstants;
    
    IDirect3DPixelShader9 *m_pPixelShader;
    ID3DXConstantTable *m_pPixelConstants;

    ID3DXBuffer *m_pTempBuffer;
    // **

    IDirect3DTexture9 **m_ppTextures;    

};

#endif // #ifdef __WIN32__
