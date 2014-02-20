#ifdef __WIN32__
#pragma once

//#include "DX9Primitives.h"

//#include "ObjectStore.h"

//class ObjectStore;

#include "DX9CmnPriv.h"

class DimensionImpDX9;

class DX9DimObject
{   
public:
    DX9DimObject()
    {
        D3DXMatrixIdentity(&m_matTransform);
    }
    // These are the important DirectX functions that
    // are needed to correctly implement a DX9 graphics 
    // object.

    virtual HRESULT DX9Init(LPDIRECT3DDEVICE9) =0;
    virtual HRESULT DX9Update() = 0;

    virtual RESULT Render(LPDIRECT3DDEVICE9) = 0;

    virtual RESULT Render(DimensionImpDX9*) = 0;

    virtual void ToString()
    {
        printf("DirectX9 Dimensional Object\n");
    }

protected:

public:
    // Transformations
    // note: sheer, scale, rotation, then translation last
    D3DXMATRIX m_matTransform;
};

#endif // #ifdef __WIN32__
