#ifdef __WIN32__
#pragma once

//#include "DX9Primitives.h"

#include "DX9DimObject.h"
#include "camera.h"

class DimensionImpDX9;

class DX9Camera :
    public camera,
    public DX9DimObject
{
public:
    DX9Camera() :
      camera()
    {
      /*empty stub*/
    }
    
    DX9Camera(point ptOrigin, point ptLookAtPosition, vector vUpDirection)  :
        camera(ptOrigin, ptLookAtPosition, vUpDirection)
    {
        
    }

    DX9Camera(DimObject *obj) :
        camera(obj->m_ptOrigin, static_cast<camera*>(obj)->GetLookAtPosition(), 
                                static_cast<camera*>(obj)->GetUpDirection())
    {
                    
    }

    ~DX9Camera()
    {

    }

    HRESULT DX9Init(LPDIRECT3DDEVICE9 pd3device9);
    HRESULT DX9Update();
    RESULT Render(DimensionImpDX9* pDimImpDX9);
    RESULT Render(LPDIRECT3DDEVICE9 pd3device9);

public:
    D3DXMATRIX GetViewMatrix(){ return m_d3dmatView; }
    D3DXMATRIX GetProjectionMatrix(){ return m_d3dmatProjection; }

private:
    D3DXMATRIX m_d3dmatView;
    D3DXMATRIX m_d3dmatProjection;
};

#endif // #ifdef __WIN32__
