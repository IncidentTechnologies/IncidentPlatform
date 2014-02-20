#ifdef __WIN32__
// DirectX9 Volume that inherits from the volume primitive
// type and implements the IDX9DimObject interface

#pragma once

//#include "DX9Primitives.h"

class DX9Volume :
    public volume,
    public DX9DimObject
{
public:
    DX9Volume() :
        volume(),
        m_pd39Buf(NULL)
    {

    }

    DX9Volume(point ptOrigin) :
        volume(ptOrigin),
        m_pd39Buf(NULL)
    {

    }

    DX9Volume(point ptOrigin, float fLength, float fHeight, float fWidth) :
        volume(ptOrigin, fLength, fHeight, fWidth),
        m_pd39Buf(NULL)
    {

    }

    DX9Volume(DimObject *obj) :
        volume(obj->m_ptOrigin, static_cast<volume*>(obj)->GetLength(), 
                                static_cast<volume*>(obj)->GetHeight(), 
                                static_cast<volume*>(obj)->GetWidth()),
        m_pd39Buf(NULL)
    {
                    
    }

    ~DX9Volume()
    {
        m_pd39Buf->Release();
    }

public:

    HRESULT DX9Init(LPDIRECT3DDEVICE9 pd3device9)
    {
        HRESULT hr = S_OK;

        pd3device9->CreateVertexBuffer
                    (
                        24*sizeof(vertex),
                        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                        CUSTOMFVF,
                        D3DPOOL_DEFAULT,
                        &m_pd39Buf,
                        NULL
                    );
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

        // lock the buffer and load in the vertices
        m_pd39Buf->Lock(0, 0, (void**)&pVoid, 0);        
            memcpy(pVoid, m_vertices, sizeof(m_vertices));
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

        // Update Vertices Transforms
        CHR(DX9Update());
        

        // Set Worldview Matrix
        pd3device9->SetTransform(D3DTS_WORLD, &(m_matTransform));
        
        pd3device9->SetFVF( CUSTOMFVF );

        pd3device9->SetStreamSource(0, m_pd39Buf, 0, sizeof(vertex));

        pd3device9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
        pd3device9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);
        pd3device9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, 2);
        pd3device9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 12, 2);       
        pd3device9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 16, 2);
        pd3device9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 20, 2);

Error:
        return r;
    }
    
public:
    IDirect3DVertexBuffer9* m_pd39Buf;
};

#endif // #ifdef __WIN32__
