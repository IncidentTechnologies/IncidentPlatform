#ifdef __WIN32__
// DirectX9 Sphere that inherits from the sphere primitive
// type and implements the IDX9DimObject interface

#pragma once

#include "point.h"
#include "sphere.h"
#include "DX9DimObject.h"

#include "DX9Material.h"

class DimensionImpDX9;

class DX9Sphere :
    public sphere,
    public DX9DimObject
{
public:
    DX9Sphere(point ptOrigin, float fRadius) :
        sphere(ptOrigin, fRadius),
        m_pd39Buf(NULL)
    {

    }

    DX9Sphere(point ptOrigin, float fRadius, int iVertical, int iAngular) :
        sphere(ptOrigin, fRadius, iVertical, iAngular),
        m_pd39Buf(NULL)
    {

    }

    DX9Sphere(DimObject *obj) :
        sphere(obj->m_ptOrigin, static_cast<sphere*>(obj)->GetRadius(), 
                                static_cast<sphere*>(obj)->GetVerticalDivisions(), 
                                static_cast<sphere*>(obj)->GetAngularDivisions(),
                                static_cast<sphere*>(obj)->GetLongSize(),
                                static_cast<sphere*>(obj)->GetLatSize()),
        m_pd39Buf(NULL)
    {
        // This is where the translation from DimObject to DX9DimObject occurs

        // Copy over the material
        if(m_pMaterial = NULL)
        {
            delete m_pMaterial;
            m_pMaterial = NULL;
        }

        //m_pMaterial = new material(static_cast<DimObject*>(obj)->m_pMaterial);
        m_pMaterial = new DX9Material(static_cast<DimObject*>(obj)->m_pMaterial);
        
    }

    ~DX9Sphere()
    {
        if(m_pd39Buf != NULL)
        {
            m_pd39Buf->Release();
        }
    }

public:

    HRESULT DX9Init(LPDIRECT3DDEVICE9 pd3device9);
    HRESULT DX9Update();
    RESULT Render(DimensionImpDX9* pDimImpDX9);
    RESULT Render(LPDIRECT3DDEVICE9 pd3device9);    // deprecated, need to remove soon
    
public:
    IDirect3DVertexBuffer9* m_pd39Buf;
    IDirect3DIndexBuffer9* m_pIndexBuffer;
};

#endif // #ifdef __WIN32__
