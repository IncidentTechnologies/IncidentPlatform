#ifdef __WIN32__
#pragma once

#include "DX9CmnPriv.h"

class DimensionImpDX9;
class DimensionImp;

class DX9FlatObject
{   
public:
    DX9FlatObject()
    {
        D3DXMatrixIdentity(&m_matTransform);
    }
    // These are the important DirectX functions that
    // are needed to correctly implement a DX9 graphics 
    // object.

    virtual RESULT Initialize(DimensionImp*) = 0;
    virtual RESULT Update() = 0;

    virtual RESULT Render(LPDIRECT3DDEVICE9) = 0;

    virtual RESULT Render(DimensionImp*) = 0;

    virtual void ToString()
    {
        printf("DirectX9 Flat Object\n");
    }

protected:
    // Transformations
    // note: sheer, scale, rotation, then translation last
    D3DXMATRIX m_matTransform;
};

#endif // #ifdef __WIN32__
