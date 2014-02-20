#ifdef __WIN32__

#include "DimensionImpDX9.h"
#include <assert.h>
#include "nuke.h"

RESULT InitializeDimObject(DimObject *pObj, void* pd3device9)
{
    dynamic_cast<DX9DimObject *>(pObj)->DX9Init((LPDIRECT3DDEVICE9)pd3device9);
    return R_OK;
}

RESULT InitializeFlatObject(FlatObject *pObj, DimensionImpDX9 *pDimDX9)
{
    RESULT r = R_OK;
    
    switch(pObj->GetObjectType())
    {
        case FLAT_COMMON_CONTROL:
        {
            CHRM(dynamic_cast<IComposite*>(pObj)->Initialize(pDimDX9), "Failed to Initialize a %s\n", pObj->ToString());
        } break;
    
        default:
        {
            CHRM(dynamic_cast<DX9FlatObject*>(pObj)->Initialize(pDimDX9), "Failed to Initialize a %s\n", pObj->ToString());
        } break;
    }   

Error:
    return r;
}

// For temporary testing purposes need to REMOVE eventually
RESULT DimensionImpDX9::InitializeDimension(ObjectStore *objStore)
{
    HRESULT hr = S_OK;

    // RENDER CODE IN HERE!        
    objStore->GetDimensionObjectDataBase()->Iterate(InitializeDimObject, (void*)(m_pd3device9));

    objStore->GetBlitQueue()->Iterate((list<FlatObject*>::FunctionPtrArg)InitializeFlatObject, (void*)(this));

Error:
    return SUCCEED;
}

RESULT DimensionImpDX9::Initialize(WindowImp &windowImp, ObjectStore *objStore)
{
    RESULT r = R_OK;
    
    HRESULT hr = S_OK;
    D3DPRESENT_PARAMETERS d3dpp;

    m_pObjectStore = objStore;  // save a pointer to the object store
    //m_pWindowImp = &windowImp;

    // create the Direct3D interface
    if((m_pd3d9 = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    {
        hr = E_FAIL;
        CHRM(hr, "DimensionImpDX9::Direct3dCreate9");
    }

    ZeroMemory(&d3dpp, sizeof(d3dpp));    
    d3dpp.Windowed = (!((bool)windowImp.getParam(WINDOW_FULLSCREEN)));
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = (HWND)(windowImp.getParam(WINDOW_HANDLE));

    // full screen stuff?
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;    // set the back buffer format to 32-bit

    d3dpp.BackBufferWidth = *((int*)(windowImp.getParam(WINDOW_WIDTH)));    // set the width of the buffer
    d3dpp.BackBufferHeight = *((int*)(windowImp.getParam(WINDOW_HEIGHT)));    // set the height of the buffer

    // Z Buffer
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    hr = m_pd3d9->CreateDevice(	D3DADAPTER_DEFAULT,
                                D3DDEVTYPE_HAL,
                                (HWND)(windowImp.getParam(WINDOW_HANDLE)),
                                D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                &d3dpp,
                                &m_pd3device9);
    CHRM(hr, "DimensionImpDX9::Failed to create directX9 device");
            
    // Set up the Shader Manager
    m_pDX9ShaderManager = new DX9ShaderManager(this);
    CNRM(m_pDX9ShaderManager, "DimensionImpDX9::Failed to initialize the shader manager!");

    // turn off lighting for now
    m_pd3device9->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pd3device9->SetRenderState(D3DRS_ZENABLE, TRUE);

    m_pd3device9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    //m_pd3device9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


    /* NOTE: We Initialize actual lighting enables when a light is created */

    InitializeDimension(objStore);

    /*
    m_pd3device9->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pd3device9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pd3device9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pd3device9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    */       

    // Initialize the projection texture
    hr = m_pd3device9->CreateTexture(PROJECTION_WIDTH,          // width
                                     PROJECTION_HEIGHT,         // height
                                     1,                         // levels (mipmaps etc)
                                     D3DUSAGE_RENDERTARGET,     // Usage (intentioned)
                                     D3DFMT_R5G6B5,             // Format 
                                     D3DPOOL_DEFAULT,           // Pool
                                     &m_pProjectionTexture,     // Texture Reference
                                     NULL                       // Handle (VISTA only boo)
                                    );

    CHRM(hr, "Could not create projection texture");
    CHRM(m_pProjectionTexture->GetSurfaceLevel(0, &m_pProjectionSurface), "Couldn't get projection surface level"); 

    // Initialize the reflection texture
    hr = m_pd3device9->CreateTexture(PROJECTION_WIDTH,          // width
                                     PROJECTION_HEIGHT,         // height
                                     1,                         // levels (mipmaps etc)
                                     D3DUSAGE_RENDERTARGET,     // Usage (intentioned)
                                     D3DFMT_R5G6B5,             // Format 
                                     D3DPOOL_DEFAULT,           // Pool
                                     &m_pReflectionTexture,     // Texture Reference
                                     NULL                       // Handle (VISTA only boo)
                                    );

    CHRM(hr, "Could not create reflection texture");
    CHRM(m_pReflectionTexture->GetSurfaceLevel(0, &m_pReflectionSurface), "Couldn't get reflection surface level");        

    // Initialize the refraction texture
    hr = m_pd3device9->CreateTexture(PROJECTION_WIDTH,          // width
                                     PROJECTION_HEIGHT,         // height
                                     1,                         // levels (mipmaps etc)
                                     D3DUSAGE_RENDERTARGET,     // Usage (intentioned)
                                     D3DFMT_R5G6B5,             // Format 
                                     D3DPOOL_DEFAULT,           // Pool
                                     &m_pRefractionTexture,     // Texture Reference
                                     NULL                       // Handle (VISTA only boo)
                                    );

    CHRM(hr, "Could not create refraction texture");
    CHRM(m_pRefractionTexture->GetSurfaceLevel(0, &m_pRefractionSurface), "Couldn't get refraction surface level"); 

    // Initialize the shadowmap texture
    hr = m_pd3device9->CreateTexture(PROJECTION_WIDTH,          // width
                                     PROJECTION_HEIGHT,         // height
                                     1,                         // levels (mipmaps etc)
                                     D3DUSAGE_RENDERTARGET,     // Usage (intentioned)
                                     D3DFMT_R5G6B5,             // Format 
                                     D3DPOOL_DEFAULT,           // Pool
                                     &m_pShadowMapTexture,      // Texture Reference
                                     NULL                       // Handle (VISTA only boo)
                                    );

    CHRM(hr, "Could not create shadowmap texture");
    CHRM(m_pShadowMapTexture->GetSurfaceLevel(0, &m_pShadowMapSurface), "Couldn't get shadow map surface level"); 

Error:
    return r;
}

RESULT RenderDimObject(DimObject *pDX9Obj, DimensionImpDX9 *pDimImpDX9, int depth, DimObject *pDX9ObjCaller, DX9Camera *pRefractionCamera, DX9Camera *pReflectionCamera)
{
    RESULT r = R_OK;
    // Note this is a directX implementation
    // so it is assumed that this is safe to do:
    
    if(pDX9Obj == pDX9ObjCaller)
        return R_EXIT;

    //pDimImpDX9->m_pd3device9->SetVertexDeclaration(pDimImpDX9->m_pD3DVertexDeclaration);

    // Update the Object
    CHRM(dynamic_cast<DX9DimObject*>(pDX9Obj)->DX9Update(), "Could not update object!");

    // Set up the shaders and materials
    // Don't render it if the shader manager returns unsupported
    r = pDimImpDX9->m_pDX9ShaderManager->SetupDX9DeviceForObject(dynamic_cast<DX9DimObject*>(pDX9Obj), depth, pDX9ObjCaller, pRefractionCamera, pReflectionCamera);    
    if(r == R_EXIT) 
        return R_OK;    
    
    /* Moving this to the shader function

    // first try the better one (the new one) eventually migrate it all over
    r = dynamic_cast<DX9DimObject*>(pDX9Obj)->Render((DimensionImpDX9*)(pDimImpDX9));

    if(r == R_NOT_IMPLEMENTED)
    {
        if(pDimImpDX9->m_pd3device9 != NULL)            
            r = dynamic_cast<DX9DimObject *>(pDX9Obj)->Render((LPDIRECT3DDEVICE9)(pDimImpDX9->GetDirectX9Device()));        
        else        
            printf("Device is NULL!");        
    }
    */


Error:
    return r;
}

RESULT RenderFlatObject(FlatObject *pObj, DimensionImpDX9 *pDimImpDX9)
{
    RESULT r = R_OK;

    switch(pObj->GetObjectType())
    {
        case FLAT_COMMON_CONTROL:
        {
            r = dynamic_cast<IComposite*>(pObj)->Render((DimensionImp *)(pDimImpDX9));

            if(r == R_NOT_IMPLEMENTED)
            {
                //assert(0);
                printf("CommonControl Render Not Implemented!\n");
            }

        } break;

        default:
        {
            // first try the better one (the new one) eventually migrate it all over (material manager stuff)
            //r = dynamic_cast<DX9FlatObject *>(pObj)->Render(reinterpret_cast<DimensionImpDX9*>(pDimImpDX9));
            r = dynamic_cast<DX9FlatObject *>(pObj)->Render(pDimImpDX9);

            if(r == R_NOT_IMPLEMENTED)     
            {
                if(pDimImpDX9->m_pd3device9 != NULL)                
                    r = dynamic_cast<DX9FlatObject*>(pObj)->Render((LPDIRECT3DDEVICE9)(pDimImpDX9->GetDirectX9Device()));                
                else                
                    printf("Device is NULL!");   
            }
        } break;
    }

Error:
    return r;
}

RESULT DimensionImpDX9::Render(ObjectStore *objStore)
{
    HRESULT hr = S_OK;
    RESULT r = R_OK;

    /*
    // Reflection Pass
    m_Reflection_f = true;      // indicate to object that we are inside of the projection
    if(m_pReflectionTexture != NULL && m_Reflection_f)
    {        
        // Lets create the reflection camera now
        // hard coded to a reflection plane in XZ with origin 0.0f   

        // Set up the clipping plane (should be moved to the shader manager implementation!)
        plane ReflectionPlane(point(0.0f, 0.0f, 0.0f), vector(0.0f, 1.0f, 0.0f), false);
        float CameraPlaneDistance = DistancePointPlane(objStore->GetMainCamera()->GetOrigin(), ReflectionPlane);
        point CameraReflectionPoint = objStore->GetMainCamera()->GetOrigin() - 2.0f * (ReflectionPlane.GetNormal() * CameraPlaneDistance);
        
        m_ReflectionCamera = DX9Camera(CameraReflectionPoint, objStore->GetMainCamera()->GetLookAtPosition(), 1.0f * objStore->GetMainCamera()->GetUpDirection());
        m_ReflectionCamera.Render(m_pd3device9);

        D3DXPLANE clipPlane;
        D3DXPlaneFromPointNormal(&clipPlane, new D3DXVECTOR3(0.0f, 0.0f, 0.0f), new D3DXVECTOR3(0.0f, 1.0f, 0.0f));
        D3DXMATRIXA16 ProjMatrix = m_ReflectionCamera.GetProjectionMatrix();
        D3DXMATRIXA16 ViewMatrix = m_ReflectionCamera.GetViewMatrix();
        D3DXMATRIXA16 WorldViewMat = ViewMatrix * ProjMatrix;
        D3DXMATRIXA16 trueClipPlane = WorldViewMat;
        D3DXMatrixInverse(&trueClipPlane, NULL, &trueClipPlane);
        D3DXMatrixTranspose(&trueClipPlane, &trueClipPlane);
        D3DXPLANE viewSpacePlane;
        D3DXPlaneTransform(&viewSpacePlane, &clipPlane, &trueClipPlane);
        m_pd3device9->SetClipPlane(0, (float *)viewSpacePlane);
        m_pd3device9->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
                
        // Set new Render target
        m_pd3device9->SetRenderTarget(0, m_pReflectionSurface);

        m_pd3device9->Clear(0,
                            NULL,
                            D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                            D3DCOLOR_XRGB(100, 100, 100),
                            1.0f,
                            0);
        // Draw the scene
        m_pd3device9->BeginScene();
        {
            // RENDER CODE IN HERE!        
            objStore->GetDimensionObjectDataBase()->Iterate((kdtree::FunctionPtrArgDepth)RenderDimObject, (void*)(this), 0);
        }
        m_pd3device9->EndScene(); 

        // Turn off the clipping plane
        m_pd3device9->SetRenderState( D3DRS_CLIPPLANEENABLE, 0x00 );
    }
    m_Reflection_f = false;
    //*/

    // Projection Pass

    // Final Pass (ALSO handle FLAT Objects here)
    // Set back to back buffer
    //m_pd3device9->SetRenderTarget(0, pBackBuffer);

    // Clear device and Z buffer
    m_pd3device9->Clear(0, 
                        NULL, 
                        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
                        D3DCOLOR_XRGB(0, 0, 0), 
                        1.0f, 
                        0
                        );

    // Reset to main camera
    //DX9Camera MainCamera(objStore->GetMainCamera());
    //MainCamera.Render(m_pd3device9);
    reinterpret_cast<DX9Camera*>(objStore->GetMainCamera())->Render(m_pd3device9);
    
    m_pd3device9->BeginScene();
    {            
        // Handle the Dimension Objects First
        objStore->GetDimensionObjectDataBase()->Iterate((kdtree::FunctionPtrArgDepthObjCams)RenderDimObject, (void*)(this), 0, NULL, NULL, NULL);

        // Handle the Flat Objects Second
        objStore->GetBlitQueue()->Iterate((list<FlatObject*>::FunctionPtrArg)RenderFlatObject, (void*)(this));
    }
    m_pd3device9->EndScene();           

    // Display the created frame
    m_pd3device9->Present(NULL, NULL, NULL, NULL);

Error:
    /*if(m_pProjectionSurface != NULL)
    {
        m_pProjectionSurface->Release();
    }

    if(pOldRenderTarget != NULL)
    {
        pOldRenderTarget->Release();
    }

    if(pOldDepthStencil != NULL)
    {
        pOldDepthStencil->Release();
    }

    if(pBackBuffer != NULL)
    {
        pBackBuffer->Release();
    }*/

    return r;
}

int DimensionImpDX9::GetWidth()
{
    if(m_pd3device9 == NULL)
    {
        // Device not initialized so the window implementation
        // width sould be appropriate
        return *(int*)m_pWindowImp->getParam(WINDOW_WIDTH);
    }
    else
    {
        D3DDISPLAYMODE d3ddm;
        HRESULT hr = m_pd3device9->GetDisplayMode(0, &d3ddm);
        if(FAILED(hr))
        {
            return -1;
        }
        else
        {
            return d3ddm.Width;
        }
    }
}

int DimensionImpDX9::GetHeight()
{
    if(m_pd3device9 == NULL)
    {
        return *(int*)m_pWindowImp->getParam(WINDOW_HEIGHT);
    }
    else
    {
        D3DDISPLAYMODE d3ddm;
        HRESULT hr = m_pd3device9->GetDisplayMode(0, &d3ddm);
        if(FAILED(hr))
        {
            return -1;
        }
        else
        {
            return d3ddm.Height;
        }
    }
}

#endif // #ifdef __WIN32__