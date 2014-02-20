#ifdef __WIN32__
// DirectX9 Triangle that inherits from the primitive type
// triangle and implements the DX9DimObject interface

// NOTE:  Officially the programmer will never need to actually
// deal with this data type.  Ideally a flag will be set and then
// dimension will do all the churning for them.  All they would need
// to do is deal with the dimension engine and the API back end 
// will finish the work

#pragma once

//#include "DX9Primitives.h"

class DX9Triangle :
    public triangle,
    public DX9DimObject
{
public:
    DX9Triangle() :
        triangle(),
        m_pd39Buf(NULL)
    {
       
    }

    DX9Triangle(int iTriStyle) :
        triangle(iTriStyle),
        m_pd39Buf(NULL)
    {

    }

    DX9Triangle(vertex *tuple) :
        triangle(tuple),
        m_pd39Buf(NULL)
    {

    }

    DX9Triangle(DimObject *obj) :
        triangle(static_cast<triangle*>(obj)->GetTuple()),
        m_pd39Buf(NULL)
    {
        
    }

    ~DX9Triangle()
    {
        m_pd39Buf->Release();
    }

public:
    HRESULT DX9Init(LPDIRECT3DDEVICE9 pd3device9)
    {
        HRESULT hr = S_OK;

        pd3device9->CreateVertexBuffer
                    (
                        3*sizeof(vertex),
                        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                        CUSTOMFVF,
                        //(D3DFVF_XYZRHW | D3DFVF_DIFFUSE),
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

        // lock the buffer and load in the vertices's
        m_pd39Buf->Lock(0, 0, (void**)&pVoid, 0);
            memcpy(pVoid, m_tuple, sizeof(m_tuple));
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

        hr = DX9Update();
        CHR(hr);

        // Set World view Matrix
        pd3device9->SetTransform(D3DTS_WORLD, &(m_matTransform));
        
        pd3device9->SetFVF( CUSTOMFVF );

        pd3device9->SetStreamSource(0, m_pd39Buf, 0, sizeof(vertex));

        pd3device9->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

Error:
        return r;
    }
    
public:
    IDirect3DVertexBuffer9* m_pd39Buf;
};

#endif // #ifdef __WIN32__
