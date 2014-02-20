#ifdef __WIN32__
#pragma once

// Height Mapped Terrain implementation for direct x

class DX9HTerrain :
    public hterrain,
    public DX9DimObject
{
public:
    DX9HTerrain() :
      hterrain(),
      m_pd39Buf(NULL),
      m_pIndexBuffer(NULL),
      m_indices(NULL)
    {

    }

    DX9HTerrain(DimObject *obj) :
      hterrain(obj->m_ptOrigin, static_cast<hterrain*>(obj)->GetWidth(),
                                static_cast<hterrain*>(obj)->GetLength(),
                                static_cast<hterrain*>(obj)->GetHeight()),
      m_pd39Buf(NULL),
      m_pIndexBuffer(NULL),
      m_indices(NULL)
    {
    
    }

    ~DX9HTerrain()
    {
        m_pd39Buf->Release();    
        m_pIndexBuffer->Release();
    }

public:
    HRESULT DX9Init(LPDIRECT3DDEVICE9 pd3device9)
    {
        HRESULT hr = S_OK;
        RESULT r = R_OK;

        pd3device9->CreateVertexBuffer
        (
            m_LongSections * m_LatSections * sizeof(vertex),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            CUSTOMFVF,
            D3DPOOL_DEFAULT,
            &m_pd39Buf,
            NULL
        );

        // Create an index buffer
        CHRM(pd3device9->CreateIndexBuffer( ((m_LongSections - 1) * (m_LatSections - 1) * 6) * sizeof(int), 
                                            D3DUSAGE_WRITEONLY,
                                            D3DFMT_INDEX32,
                                            D3DPOOL_MANAGED,
                                            &m_pIndexBuffer,
                                            NULL),
                                            "DX9HTerrain::Could not create index buffer!"
                                          );

        // INDEX BUFFER STUFF (NEWS TO ME!)

        // set up the indices
        m_indices = new int[((m_LongSections - 1) * (m_LatSections - 1) * 6)];

        // fill up the index buffer
        for(int i = 0; i < m_LatSections - 1; i++)
        {
            for(int j = 0; j < m_LongSections - 1; j++)
            {                
                m_indices[(i * (m_LatSections - 1) + j) * 6 + 2] = (i * m_LatSections) + j;
                m_indices[(i * (m_LatSections - 1) + j) * 6 + 1] = (i * m_LatSections) + (j + 1);
                m_indices[(i * (m_LatSections - 1) + j) * 6 + 0] = ((i + 1) * m_LatSections) + (j + 1);                
                
                m_indices[(i * (m_LatSections - 1) + j) * 6 + 3] = ((i + 1) * m_LatSections) + (j + 1);
                m_indices[(i * (m_LatSections - 1) + j) * 6 + 4] = (i * m_LatSections) + j;
                m_indices[(i * (m_LatSections - 1) + j) * 6 + 5] = ((i + 1) * m_LatSections) + j;
            }
        }

        // Need to migrate this out into the directX implementation!
        // **
        const D3DVERTEXELEMENT9 d3dvertexdecl[] = 
        {
            {0,     0,      D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0},
            {0,     12,     D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,    0},
            {0,     24,     D3DDECLTYPE_D3DCOLOR,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR,     0},
            //{0,     28,     D3DDECLTYPE_FLOAT2,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0},
            D3DDECL_END()
        };

        hr = pd3device9->CreateVertexDeclaration(d3dvertexdecl, &m_pVertexDecl); 
        if(FAILED(hr))
        {
            printf("dx9hterrain::Could not create vertex declaration\n");
        }
        // **

        // Texture Loading (maybe do this in DXDimObject?
        if(pd3device9 != NULL)
        {        
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
                    }   
                }
            }
        }
                
Error:
        return hr;
    }

    HRESULT DX9Update()
    {
        HRESULT hr = S_OK;
        RESULT r = R_OK;
        
        void* pVoid;
        void *pIndices;

        static float offset = 0.0f;

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
            memcpy(pVoid, m_Vertices, sizeof(vertex) * m_LongSections * m_LatSections);
        m_pd39Buf->Unlock();        

        m_pIndexBuffer->Lock(0, ((m_LongSections - 1) * (m_LatSections - 1) * 6) * sizeof(int), (void**)&pIndices, 0);
            memcpy(pIndices, m_indices, ((m_LongSections - 1) * (m_LatSections - 1) * 6) * sizeof(int));
        m_pIndexBuffer->Unlock();

Error:
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

        hr = DX9Update();
        CHR(hr);

        //pd3device9->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);

        //pd3device9->SetRenderState(D3DRS_LIGHTING, FALSE);
        // Set World view Matrix
        pd3device9->SetTransform(D3DTS_WORLD, &(m_matTransform));

        //pd3device9->SetVertexDeclaration(m_pVertexDecl);
        pd3device9->SetFVF( CUSTOMFVF );

        pd3device9->SetStreamSource(0, m_pd39Buf, 0, sizeof(vertex));
        pd3device9->SetIndices(m_pIndexBuffer);

        if(GetTextureCount(TEXTURE_DIFFUSE) > 0)
        {
            pd3device9->SetTexture(0, m_ppTextures[0]);        
        }
        else
        {
            pd3device9->SetTexture(0, NULL);
        }

        //pd3device9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_LongSections);
        pd3device9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_LongSections * m_LatSections, 0, ((m_LongSections - 1) * (m_LatSections - 1)) * 2);

Error:
        return r;
    }

public:
    IDirect3DVertexBuffer9* m_pd39Buf;
    IDirect3DIndexBuffer9* m_pIndexBuffer;
    IDirect3DVertexDeclaration9 *m_pVertexDecl;

    IDirect3DTexture9 **m_ppTextures;  

    int *m_indices;
};

#endif // #ifdef __WIN32__
