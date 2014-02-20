#ifdef __WIN32__

#include "DX9Camera.h"
#include "DimensionImpDX9.h"


RESULT DX9Camera::Render( LPDIRECT3DDEVICE9 pd3device9 )
{
    RESULT r = R_OK;

    // Update The Camera Transformations
    HRESULT hr = DX9Update();
    CHR(hr);

    // Set the view transform
    pd3device9->SetTransform(D3DTS_VIEW, &m_d3dmatView);      

    D3DXMatrixPerspectiveFovLH( &m_d3dmatProjection,                            // Projection Matrix
                                D3DXToRadian(45),                               // the horizontal field of view
                                (FLOAT)DEFAULT_WIDTH / (FLOAT)DEFAULT_HEIGHT,   // aspect ratio
                                1.0f,                                           // the near view-plane
                                250.0f                                          // the far view-plane
                              );       



    // Set the Projection Transform
    pd3device9->SetTransform(D3DTS_PROJECTION, &m_d3dmatProjection);

Error:
    return r;
}

RESULT DX9Camera::Render( DimensionImpDX9* pDimImpDX9 )
{
    IDirect3DDevice9 *pDevice = pDimImpDX9->m_pd3device9;

    if(pDimImpDX9->m_Projection_f)
    {
        // Do not render on projection pass!
        return R_OK;
    }

    //return R_NOT_IMPLEMENTED;
    return R_OK;
}

HRESULT DX9Camera::DX9Update()
{
    HRESULT hr = S_OK;

    D3DXMatrixLookAtLH( &m_d3dmatView,
                        &D3DXVECTOR3 (m_ptOrigin.tuple[0], m_ptOrigin.tuple[1], m_ptOrigin.tuple[2]),                           // Camera Origin
                        &D3DXVECTOR3 (m_ptLookAtPosition.tuple[0], m_ptLookAtPosition.tuple[1], m_ptLookAtPosition.tuple[2]),   // Look At Position
                        &D3DXVECTOR3 (m_vUpDirection.tuple[0], m_vUpDirection.tuple[1], m_vUpDirection.tuple[2])                // Up Direction
                      );            

Error:
    return hr;
}

HRESULT DX9Camera::DX9Init( LPDIRECT3DDEVICE9 pd3device9 )
{
    // Set up transform first 
    DX9Update();
    Render(pd3device9);

    return S_OK;
}

#endif // #ifdef __WIN32__
