#ifdef __WIN32__
#pragma once

// Dimension Implementation for DirectX 9

// NOTE:  Remember to add :
// C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include
// C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86
// to the VC++ directories for include / libraries

#include "DimensionImp.h"
#include "cmnPriv.h"
#include "ObjectStore.h"
#include "DX9ShaderManager.h"

class ObjectStore;
class DimensionImp;
class DX9ShaderManager;

#ifndef _DIMDX9
#define _DIMDX9

#include <d3d9.h>
#include "WindowWin32.h"
#pragma comment(lib, "d3d9.lib")
#ifdef _DEBUG
    #pragma comment(lib,"d3dx9d.lib")
#else
    #pragma comment(lib,"d3dx9.lib")
#endif
#endif

#include "DX9Camera.h"

class DX9Camera;

RESULT RenderDimObject(DimObject *pDX9Obj, DimensionImpDX9 *pDimImpDX9, int depth, DimObject *pDX9ObjCaller, DX9Camera *pRefractionCamera, DX9Camera *pReflectionCamera);
RESULT RenderFlatObject(FlatObject *pObj, DimensionImpDX9 *pDimImpDX9);

class DimensionImpDX9 :
    public DimensionImp
{
public:
    DimensionImpDX9(DIMENSION_TYPE dt, WindowImp *pWindowImp) :
      m_pd3d9(NULL),
      m_pd3device9(NULL),
      m_pWindowImp(pWindowImp),

      m_pShadowMapSurface(NULL),
      m_pShadowMapTexture(NULL),
      m_ShadowMap_f(false),

      m_pReflectionSurface(NULL),
      m_pReflectionTexture(NULL),
      m_Reflection_f(false),

      m_pRefractionSurface(NULL),
      m_pRefractionTexture(NULL),
      m_Refraction_f(false),

      m_pProjectionSurface(NULL),
      m_pProjectionTexture(NULL),
      m_Projection_f(false)
    {
        this->m_DimType = dt;
    }

    ~DimensionImpDX9(){ /*empty stub*/ }

    RESULT Initialize(WindowImp &windowImp, ObjectStore *objStore);
    RESULT Render(ObjectStore *objStore);
    RESULT InitializeDimension(ObjectStore *objStore);   // <-- need to remove eventually
    ObjectStore* GetObjectStore(){ return m_pObjectStore; }
    IDirect3DDevice9* GetDirectX9Device(){ return m_pd3device9; }
    
    int GetWidth();
    int GetHeight();    

private:
    LPDIRECT3D9 m_pd3d9;
    WindowImp *m_pWindowImp;            // handle to owner window implementation

public:
    // DirectX9 Device
    LPDIRECT3DDEVICE9 m_pd3device9;
    ObjectStore *m_pObjectStore;        // Save a pointer to the object store

public:    
    // iEngine DX9 Shader Manager
    DX9ShaderManager *m_pDX9ShaderManager;

public:
    // Should come up with a more flexible way to do this especially
    // when we start talking about multiple shadows and multiple 
    // reflections / refractions in a scene (or projections!)

    IDirect3DSurface9 *m_pShadowMapSurface;
    IDirect3DTexture9 *m_pShadowMapTexture;
    DX9Camera m_ShadowMapCamera;
    bool m_ShadowMap_f;

    IDirect3DSurface9 *m_pReflectionSurface;
    IDirect3DTexture9 *m_pReflectionTexture;
    DX9Camera m_ReflectionCamera;
    bool m_Reflection_f;

    IDirect3DSurface9 *m_pRefractionSurface;
    IDirect3DTexture9 *m_pRefractionTexture;
    DX9Camera m_RefractionCamera;               // not sure if this is needed since this is usually the view point with a different clipping plane
    bool m_Refraction_f;

    // Testing it out first
    IDirect3DSurface9 *m_pProjectionSurface;
    IDirect3DTexture9 *m_pProjectionTexture;
    DX9Camera m_ProjectionCamera;
    bool m_Projection_f;

    bool m_FinalPass_f;
};

#endif // __WIN32__