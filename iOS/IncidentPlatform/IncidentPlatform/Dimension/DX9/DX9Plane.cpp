#ifdef __WIN32__
#include "DX9Plane.h"

#include "DimensionImpDX9.h"

// This is the one used for passing in texture info from
// the DimensionImpDX9
// Eventually need to migrate over to only this function
RESULT DX9Plane::Render( DimensionImpDX9 *pDimImpDX9 )
{
    RESULT r = R_OK;

    IDirect3DDevice9 *pDevice = pDimImpDX9->m_pd3device9;   // easy access
    CNRM(pDevice, "DX9Plane::Render::Device is NULL!");

    /*
    if(pDimImpDX9->m_Projection_f || 
       pDimImpDX9->m_Reflection_f ||
       pDimImpDX9->m_Refraction_f )
    {
        // This needs to be controlled through the shader manager or dimension implementation
        // Make sure not to render this inside of the
        // projection rendering pass  
        //return R_NOT_IMPLEMENTED;
        return R_OK;
    }
    else 
    {    */          
        // Render for reals!
        //CHRM(DX9Update(), "Render::Failed to DX9Update");  // update is in the implementation now
        
        // Set the geometry stream source
        pDevice->SetStreamSource(0, m_pd39Buf, 0, sizeof(vertex));
        pDevice->SetIndices(m_pIndexBuffer);

        // Pass the geometry data to the GPU
        pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);  
        pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_LongSections * m_LatSections, 0, ((m_LongSections - 1) * (m_LatSections - 1)) * 2);
        pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); 
    //}

Error:
    return r;
}

RESULT DX9Plane::Render( LPDIRECT3DDEVICE9 pd3device9 )
{
    return R_NOT_IMPLEMENTED;
}

HRESULT DX9Plane::DX9Update()
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


HRESULT DX9Plane::DX9Init( LPDIRECT3DDEVICE9 pd3device9 )
{
    HRESULT hr = S_OK;
    RESULT r = R_OK;

    pd3device9->CreateVertexBuffer( m_LongSections * m_LatSections * sizeof(vertex),
                                    D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                                    CUSTOMFVF,
                                    D3DPOOL_DEFAULT,
                                    &m_pd39Buf,
                                    NULL );

    // Create an index buffer
    CHRM(pd3device9->CreateIndexBuffer( ((m_LongSections - 1) * (m_LatSections - 1) * 6) * sizeof(int), 
                                        D3DUSAGE_WRITEONLY,
                                        D3DFMT_INDEX32,
                                        D3DPOOL_MANAGED,
                                        &m_pIndexBuffer,
                                        NULL),
                                        "DX9HTerrain::Could not create index buffer!" );

    

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

    CRM(static_cast<DX9Material*>(m_pMaterial)->Initialize(pd3device9), "DX9Plane: Failed to Initialize Material");

Error:
    return hr;
}

#endif // #ifdef __WIN32__
