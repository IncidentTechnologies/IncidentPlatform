#ifdef __WIN32__
#include "DX9Sphere.h"
#include "DimensionImpDX9.h"


HRESULT DX9Sphere::DX9Init( LPDIRECT3DDEVICE9 pd3device9 )
{
    HRESULT hr = S_OK;
    RESULT r = R_OK;

    CHRM(pd3device9->CreateVertexBuffer
        (
        m_cVerts * sizeof(vertex),
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
        CUSTOMFVF,
        D3DPOOL_DEFAULT,
        &m_pd39Buf,
        NULL
        ),
        "DX9Sphere::Could not create vertex buffer!"
        );


    CHRM(pd3device9->CreateIndexBuffer( m_indices_n * sizeof(int), 
        D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX32,
        D3DPOOL_MANAGED,
        &m_pIndexBuffer,
        NULL),
        "DX9Sphere::Could not create index buffer!"
        );    


    CRM(static_cast<DX9Material*>(m_pMaterial)->InitializeTextures(pd3device9), "DX9Shpere: Failed to Initialize Textures");

Error:
    return hr;
}

HRESULT DX9Sphere::DX9Update()
{
    HRESULT hr = S_OK;
    void* pVoid;
    void *pIndices;

    D3DXMATRIX matTemp;

    D3DXMatrixIdentity(&m_matTransform);

    if( m_fScale != 1.0f )
    {   
        // Scale only if scale is not 1
        D3DXMatrixScaling(&matTemp, m_fScale, m_fScale, m_fScale);
        D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
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
        D3DXMatrixTranslation(&matTemp, m_ptOrigin.tuple[DIM_X], 
            m_ptOrigin.tuple[DIM_Y], 
            m_ptOrigin.tuple[DIM_Z]);

        D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
    }

    // lock the buffer and load in the vertices
    m_pd39Buf->Lock(0, 0, (void**)&pVoid, 0);        
        memcpy(pVoid, m_verts, m_cVerts * sizeof(vertex));
    m_pd39Buf->Unlock();

    // Copy Index Buffer
    m_pIndexBuffer->Lock(0, m_indices_n * sizeof(int), (void**)&pIndices, 0);
        memcpy(pIndices, m_indices, m_indices_n * sizeof(int));
    m_pIndexBuffer->Unlock();

    return hr;
}

RESULT DX9Sphere::Render( DimensionImpDX9* pDimImpDX9 )
{
    RESULT r = R_OK;

    IDirect3DDevice9 *pDevice = pDimImpDX9->m_pd3device9;   // easy access
    CNRM(pDevice, "DX9Plane::Render::Device is NULL!");

    // Set up the geometry source
    pDevice->SetStreamSource(0, m_pd39Buf, 0, sizeof(vertex));
    pDevice->SetIndices(m_pIndexBuffer);

    float num = (2 + m_iVerticalDivisions * m_iAngularDivisions);  

    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);        
    pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, m_cVerts, 0, m_iAngularDivisions );       
    pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_cVerts, (m_iAngularDivisions + 2), (m_iVerticalDivisions - 1) * m_iAngularDivisions * 2);

    if(m_LongSize == 1.0f)
        pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, m_cVerts, m_indices_n - m_iAngularDivisions - 2, m_iAngularDivisions);

    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);


Error:
    return r;
}

RESULT DX9Sphere::Render( LPDIRECT3DDEVICE9 pd3device9 )
{
    return R_NOT_IMPLEMENTED;
}

#endif // #ifdef __WIN32__
