#ifdef __WIN32__
#pragma once

// Direct X 9 MD3 Mesh implementation
// This will totally encapsulate the direct x 9 implementation of the MD3 file format

class DX9MeshMd3 :
    public MeshMd3,
    public DX9DimObject
{
public:
    DX9MeshMd3() :
      MeshMd3(),
      m_pd39Buf(NULL),
      m_Triangles_n(0)
    {

    }

    DX9MeshMd3(char *pszFilename) :
      MeshMd3(pszFilename),
      m_pd39Buf(NULL),
      m_Triangles_n(0)
    {

    }

    DX9MeshMd3(DimObject *obj) :
      MeshMd3(static_cast<MeshMd3*>(obj)->GetPath(), 
              static_cast<MeshMd3*>(obj)->GetFilename()),
      m_pd39Buf(NULL),
      m_Triangles_n(0)
    {

    }

    ~DX9MeshMd3()
    {
        m_Triangles_n = 0;
        m_pd39Buf->Release();
    }

public:
    HRESULT DX9Init(LPDIRECT3DDEVICE9 pd3device9)
    {
        RESULT r = R_OK;
        HRESULT hr = S_OK;

        if(m_surfaces != NULL)
        {        
            // first calculate how many vertices we have
            m_Triangles_n = 0;
            for(int i = 0; i < m_header.NumSurfaces; i++)
            {
                m_Triangles_n += m_surfaces[i].NumTriangles;
            }
            
            pd3device9->CreateVertexBuffer
            (
                //RECTANGLE_VERTEX_COUNT * sizeof(vertex),
                //m_surfaces[0].NumTriangles * 3 * sizeof(vertex),
                m_Triangles_n * 3 * sizeof(vertex),
                D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                CUSTOMFVF,
                D3DPOOL_DEFAULT,
                &m_pd39Buf,
                NULL
            );
        }
        else
        {
            printf("Md3 file not initialized!\n");
            return E_FAIL;
        }

        if(pd3device9 != NULL)
        {        
            //D3DXCreateTextureFromFile(pd3device9, L"./data/Metal.png", &m_pTexture);
            m_ppTextures = new IDirect3DTexture9*[GetTextureCount(TEXTURE_DIFFUSE)];

            for(int i = 0; i < GetTextureCount(TEXTURE_DIFFUSE); i++)
            {
                
                char *TempPath = NULL;
                r = GetTexture(TEXTURE_DIFFUSE).GetTexture(i, TempPath);

                BSTR TexturePath;

                int lenA = lstrlenA(TempPath);
                int lenW;

                lenW = MultiByteToWideChar(CP_ACP, NULL, TempPath, -1, NULL, NULL);
                
                if(lenW > 0) 
                    printf("Failed to allocation BSTR!\n");
                                
                TexturePath = SysAllocStringLen(0, lenW);
                MultiByteToWideChar(CP_ACP, NULL, TempPath, -1, TexturePath, lenW);               

                hr = D3DXCreateTextureFromFile(pd3device9, TexturePath, &m_ppTextures[i]);

                if(m_ppTextures[i] == NULL || FAILED(hr))
                {
                    printf("Failed to create texture %d\n", i);
                }   
            }

            // To keep from crashing and burning we assume that not all textures are loaded correctly
            // in this case we load dummy data that we know to be correct 
            if(GetTextureCount(TEXTURE_DIFFUSE) < m_header.NumSurfaces)
            {
                for(int i = GetTextureCount(TEXTURE_DIFFUSE); i < m_header.NumSurfaces; i++)
                {
                    // For now lets just assign some 
                    // standard texture for surface shaders not found
                    D3DXCreateTextureFromFile(pd3device9, L"./data/glad/S_LOWER.TGA", &m_ppTextures[i]);
                }
            }
            //D3DXCreateTextureFromFile(pd3device9, L"./data/laracroft/default.tga", &m_ppTextures[0]);
            //D3DXCreateTextureFromFile(pd3device9, L"./data/laracroft/default.tga", &m_ppTextures[1]);
        }

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

Error:
        return hr;
    }

    HRESULT DX9Update() {
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


#define START_FRAME 107
#define FRAME_LENGTH 12
        // lock the buffer and load in the vertices's
        m_pd39Buf->Lock(0, 0, (void**)&pVoid, 0);
            // Only doing the first surface for now
            int TriCount = 0;
            
            if(m_Animation_f) {
                m_AnimationProgress += m_AnimationSpeed;

                if(m_AnimationProgress >= 1.0f)
                    m_AnimationProgress = 0.0f;
            }
                
            int LeftFrame = m_AnimationStartFrame + (int)(m_AnimationProgress * m_AnimationFrameLength);
            float Proportion = (m_AnimationStartFrame + m_AnimationProgress * m_AnimationFrameLength) - LeftFrame;
            int RightFrame = LeftFrame + 1;           

            for(int i = 0; i < m_header.NumSurfaces; i++) {
                vertex TempVertex[3];
                for(int j = 0; j < m_surfaces[i].NumTriangles; j++) {
                    int c = m_triangles[i][j].Indices[0];
                    int b = m_triangles[i][j].Indices[1];
                    int a = m_triangles[i][j].Indices[2];

                    TempVertex[0] = m_DimensionVertices[i][a + LeftFrame * m_surfaces[i].NumVertices];
                    TempVertex[1] = m_DimensionVertices[i][b + LeftFrame * m_surfaces[i].NumVertices];
                    TempVertex[2] = m_DimensionVertices[i][c + LeftFrame * m_surfaces[i].NumVertices];

                    if(Proportion != 0)
                    {                        
                        TempVertex[0].set(TempVertex[0] + ((m_DimensionVertices[i][a + RightFrame * m_surfaces[i].NumVertices] - TempVertex[0]) * Proportion));
                        TempVertex[1].set(TempVertex[1] + ((m_DimensionVertices[i][b + RightFrame * m_surfaces[i].NumVertices] - TempVertex[1]) * Proportion));
                        TempVertex[2].set(TempVertex[2] + ((m_DimensionVertices[i][c + RightFrame * m_surfaces[i].NumVertices] - TempVertex[2]) * Proportion));                                                                
                    }

                    TempVertex[0].SetColor(1.0f, 1.0f, 1.0f, 1.0f);
                    TempVertex[1].SetColor(1.0f, 1.0f, 1.0f, 1.0f);
                    TempVertex[2].SetColor(1.0f, 1.0f, 1.0f, 1.0f);                
                
                    memcpy((void*)((int)pVoid + (TriCount * 3 * sizeof(vertex))), TempVertex, sizeof(vertex) * 3);
                    TriCount++;
                }
            }

        m_pd39Buf->Unlock();

        return hr;
    }

//#define MD3_WIREFRAME
//#define MD3_NOLIGHT

    RESULT Render(DimensionImpDX9* pDimImpDX9) {
        return R_NOT_IMPLEMENTED;
    }

    RESULT Render(LPDIRECT3DDEVICE9 pd3device9) {
        RESULT r = R_OK;
        HRESULT hr = S_OK;   

        hr = DX9Update();
        CHR(hr);
        
        #ifdef MD3_NOLIGHT
            pd3device9->SetRenderState(D3DRS_LIGHTING, FALSE);
        #endif

        // Set World view Matrix
        pd3device9->SetTransform(D3DTS_WORLD, &(m_matTransform));

        //pd3device9->SetFVF( CUSTOMFVF );

        pd3device9->SetVertexDeclaration(m_pVertexDecl);

        pd3device9->SetStreamSource(0, m_pd39Buf, 0, sizeof(vertex));

        int TriCount = 0;
        for(int i = 0; i < m_header.NumSurfaces; i++) {
            pd3device9->SetTexture(0, m_ppTextures[i]);
        
            #ifdef MD3_WIREFRAME
                pd3device9->DrawPrimitive(D3DPT_LINESTRIP, TriCount, m_Surfaces[i].NumTriangles);
            #else
                pd3device9->DrawPrimitive(D3DPT_TRIANGLELIST, TriCount * 3, m_surfaces[i].NumTriangles);
            #endif

            TriCount += m_surfaces[i].NumTriangles;
        }

        #ifdef MD3_NOLIGHT
            pd3device9->SetRenderState(D3DRS_LIGHTING, TRUE);
        #endif

Error:
        return r;
    }

public:
    IDirect3DVertexBuffer9* m_pd39Buf;
    IDirect3DTexture9 **m_ppTextures;    


    IDirect3DVertexDeclaration9 *m_pVertexDecl;

private:    
    int m_Triangles_n;
};

#endif // #ifdef __WIN32__
