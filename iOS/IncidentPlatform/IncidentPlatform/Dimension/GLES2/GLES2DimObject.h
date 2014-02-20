#pragma once

//#include "GLES2CmnPriv.h"

//class DimensionImpDX9;

#include "EHM.h"

using namespace dss;

class GLES2DimObject {
public:
    GLES2DimObject() {
        //D3DXMatrixIdentity(&m_matTransform);
    }
    // These are the important DirectX functions that
    // are needed to correctly implement a DX9 graphics 
    // object.

    virtual RESULT GLES2Init() =0;
    virtual RESULT GLES2Update(GLuint modelView) = 0;

    //virtual RESULT Render(LPDIRECT3DDEVICE9) = 0;
    
    virtual RESULT Render(GLuint position, GLuint color) = 0;

    virtual void ToString() {
        printf("OpenGL ES 2.0 Dimensional Object\n");
    }

protected:

public:
    // Transformations
    // note: sheer, scale, rotation, then translation last
    //D3DXMATRIX m_matTransform;
};

