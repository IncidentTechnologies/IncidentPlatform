#ifdef __WIN32__
#include "DX9ShaderManager.h"
#include "DimensionImpDX9.h"

RESULT DX9ShaderManager::ShadePhongRefract( DX9DimObject *pObj, int depth, DimObject *pDX9ObjCaller, 
                                            IDirect3DVertexShader9 *pVertexShader, ID3DXConstantTable *pVertexConstants, 
                                            IDirect3DPixelShader9 *pPixelShader, ID3DXConstantTable *pPixelConstants,
                                            DX9Camera *pRefractionCamera, DX9Camera *pReflectionCamera
                                          )
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;
    
    D3DXMATRIXA16 matWorldViewProj = GetWorldViewProjMat();

    if(depth >= DX9_SHADER_MAX_DEPTH)
        return R_EXIT;

    // First we need to decide on what kind of shader this material is
    DimObject *pDimObj = dynamic_cast<DimObject*>(pObj);    // need to acquire the DimObject Interface out of the DX9DimObject
    
    // Save back buffer and depth stencil for later
    IDirect3DSurface9 *pBackBuffer = NULL;      
    IDirect3DSurface9 *pDepthBuffer = NULL;
    m_pd3device9->GetRenderTarget(0, &pBackBuffer);
    m_pd3device9->GetDepthStencilSurface(&pDepthBuffer);

    IDirect3DTexture9 *pRefractionTexture = NULL;
    IDirect3DSurface9 *pRefractionSurface = NULL;
    IDirect3DSurface9 *pRefractionDepthStencil = NULL;

    // Initialize the reflection texture
    hr = m_pd3device9->CreateTexture(   PROJECTION_WIDTH,          // width
                                        PROJECTION_HEIGHT,         // height
                                        1,                         // levels (mipmaps etc)
                                        D3DUSAGE_RENDERTARGET,     // Usage (intentioned)
                                        D3DFMT_R5G6B5,             // Format 
                                        D3DPOOL_DEFAULT,           // Pool
                                        &pRefractionTexture,     // Texture Reference
                                        NULL                       // Handle (VISTA only boo)
                                        );

    CHRM(hr, "ShadePhongRefract: Could not create reflraction texture");
    CHRM(pRefractionTexture->GetSurfaceLevel(0, &pRefractionSurface), "ShadePhongRefract: Couldn't get refraction surface level");   
    
    D3DSURFACE_DESC Desc;
    CHRM(pDepthBuffer->GetDesc(&Desc), "ShadePhongRefract: Failed to get depth stencil buffer description");
    
    hr = m_pd3device9->CreateDepthStencilSurface(   Desc.Width, 
                                                    Desc.Height, 
                                                    Desc.Format, 
                                                    Desc.MultiSampleType, 
                                                    Desc.MultiSampleQuality, 
                                                    false,
                                                    &pRefractionDepthStencil, 
                                                    NULL
                                                );

    CHRM(hr, "ShadePhongRefract: Failed to create refraction depth stencil buffer");

    // Get a handle to the object store
    ObjectStore *pObjectStore = m_pDimImpDX9->GetObjectStore();                      

    // Set new Render target
    m_pd3device9->SetRenderTarget(0, pRefractionSurface);
    m_pd3device9->SetDepthStencilSurface(pRefractionDepthStencil);

    if(pRefractionTexture != NULL)
    {                        
        // Set up the clipping plane 
        // Set up the clipping plane (should be moved to the shader manager implementation!)
        plane *pRefractionPlane = pDimObj->GetPlane();
        if(pRefractionPlane == NULL)            
            pRefractionPlane = new plane(point(0.0f, 0.0f, 0.0f), vector(0.0f, 1.0f, 0.0f), false, false); 
        vector RefractNormal = pRefractionPlane->GetNormal() * -1.0f;

        D3DXMATRIXA16 ProjMatrix;
        D3DXMATRIXA16 ViewMatrix;

        // The refraction camera will always be the view point of the main
        // camera with a different clipping plane
        if(pDX9ObjCaller != NULL)
        {
            if(pDX9ObjCaller->GetMaterialShadeStyle() == SHADE_PHONG_REFLECT && pReflectionCamera != NULL)
            {               
                pReflectionCamera->Render(m_pd3device9);

                ProjMatrix = pReflectionCamera->GetProjectionMatrix();
                ViewMatrix = pReflectionCamera->GetViewMatrix();

                RefractNormal = RefractNormal * -1.0f;  // flip the clipping plane
            }
            else if(pDX9ObjCaller->GetMaterialShadeStyle() == SHADE_PHONG_REFRACT && pRefractionCamera != NULL)
            {
                pRefractionCamera->Render(m_pd3device9);
                ProjMatrix = pRefractionCamera->GetProjectionMatrix();
                ViewMatrix = pRefractionCamera->GetViewMatrix();
            }
        }
        else
        {
            pRefractionCamera = reinterpret_cast<DX9Camera*>(pObjectStore->GetMainCamera());  
            pRefractionCamera->Render(m_pd3device9);

            ProjMatrix = pRefractionCamera->GetProjectionMatrix();
            ViewMatrix = pRefractionCamera->GetViewMatrix();
        }

        D3DXPLANE clipPlane;                                
        D3DXPlaneFromPointNormal(&clipPlane, &D3DXVECTOR3(pXYZ(pRefractionPlane->m_ptOrigin)), &D3DXVECTOR3(vXYZ(RefractNormal)));                                

        D3DXMATRIXA16 WorldViewMat = ViewMatrix * ProjMatrix;
        D3DXMATRIXA16 trueClipPlane = WorldViewMat;
        D3DXMatrixInverse(&trueClipPlane, NULL, &trueClipPlane);
        D3DXMatrixTranspose(&trueClipPlane, &trueClipPlane);
        D3DXPLANE viewSpacePlane;
        D3DXPlaneTransform(&viewSpacePlane, &clipPlane, &trueClipPlane);   

        m_pd3device9->SetClipPlane(0, (float *)viewSpacePlane);                                        
        m_pd3device9->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);                 

        hr = m_pd3device9->Clear(0,
            NULL,
            D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
            D3DCOLOR_XRGB(0, 0, 0),
            1.0f,
            0);

        // Draw the scene                    
        m_pd3device9->BeginScene();
        {
            // Render the rest of the scene incrementing the depth and avoiding self refraction by passing
            // the object as the caller.  So when the render function is reached for this object it will be skipped
            pObjectStore->GetDimensionObjectDataBase()->Iterate((kdtree::FunctionPtrArgDepthObjCams)RenderDimObject, m_pDimImpDX9, depth + 1, pDimObj, pRefractionCamera, NULL);
        }
        m_pd3device9->EndScene();             

        // Turn off the clipping plane
        m_pd3device9->SetRenderState( D3DRS_CLIPPLANEENABLE, 0x00 );    

        if(pDimObj->GetPlane() == NULL)
            delete pRefractionPlane;
    }
    // Now we reset the back buffer and the depth stencil        
    m_pd3device9->SetRenderTarget(0, pBackBuffer);  
    m_pd3device9->SetDepthStencilSurface(pDepthBuffer);

    // release to avoid memory leak
    pDepthBuffer->Release();
    pBackBuffer->Release();

    // reset camera 
    if(pDX9ObjCaller != NULL && pDX9ObjCaller->GetMaterialShadeStyle() == SHADE_PHONG_REFLECT && pReflectionCamera != NULL)
    {
        pReflectionCamera->Render(m_pd3device9);
    }
    else
    {
        pRefractionCamera->Render(m_pd3device9);
    }

    // Pass the World View Projection Matrix and object transformation matrices to the Vertex Shader
    pVertexConstants->SetMatrix(m_pd3device9, "g_WorldViewProj", &matWorldViewProj); 
    pVertexConstants->SetMatrix(m_pd3device9, "g_Transform", &(pObj->m_matTransform));

    // FAKE THE LIGHTING for now
    static float theta = 0.0f;
    //theta -= .005;
    float radius = 2.5f;
    float LightXVal = radius * sin(theta);
    LightXVal = 2.5f;
    float LightZVal = radius * cos(theta);

    // NEED TO CALL lighting/camera manager here in reality
    pVertexConstants->SetFloatArray(m_pd3device9, "g_ViewPosition", point(0.0f, 5.0f, 0.0f).tuple, 3);
    pVertexConstants->SetFloatArray(m_pd3device9, "g_light0", point(LightXVal, 2.5f, LightZVal).tuple, 3);

    static float mult = 0.0f;
    mult += .00025;
    pPixelConstants->SetFloat(m_pd3device9, "g_Multiplier", abs(sin(mult)));

    // Set up some refraction specific details (need the implementation)
    pPixelConstants->SetBool(m_pd3device9, "g_Refraction", true);

    D3DXMATRIXA16 RefractionCamView;
    D3DXMATRIXA16 OriginalCamProjection;

    D3DXMATRIXA16 TrafoMatrix(  0.5f,  0.0f, 0.0f, 0.0f, 
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f,  0.0f, 0.0f, 0.0f, 
        0.5f,  0.5f, 0.0f, 1.0f );

    if(pDX9ObjCaller != NULL && pReflectionCamera != NULL)
    {
        RefractionCamView = pReflectionCamera->GetViewMatrix();
        OriginalCamProjection = pReflectionCamera->GetProjectionMatrix();
    }
    else
    {
        RefractionCamView = pRefractionCamera->GetViewMatrix();
        OriginalCamProjection = pRefractionCamera->GetProjectionMatrix();
    }

    D3DXMATRIXA16 RefractionProjectionMatrix = RefractionCamView * OriginalCamProjection * TrafoMatrix;
    pVertexConstants->SetMatrix(m_pd3device9, "g_RefractionProjectionMatrix", &RefractionProjectionMatrix);

    // Pass the shaders to the device
    m_pd3device9->SetVertexShader(pVertexShader);
    m_pd3device9->SetPixelShader(pPixelShader);

    // Pass the diffuse, bump textures to the device
    CRM(static_cast<DX9Material*>(pDimObj->m_pMaterial)->SetTextures(m_pd3device9, TEXTURE_DIFFUSE ), "DX9ShaderManager: Failed to Set Diffuse Textures for Rendering");
    CRM(static_cast<DX9Material*>(pDimObj->m_pMaterial)->SetTextures(m_pd3device9, TEXTURE_BUMP), "DX9ShaderManager: Failed to Set Bump Textures for Rendering");           

    // Refract textures
    //if(static_cast<DX9Material*>(pDimObj->m_pMaterial)->m_pRefractionTexture != NULL)                 
    //    m_pd3device9->SetTexture(0, static_cast<DX9Material*>(pDimObj->m_pMaterial)->m_pRefractionTexture);  

    if(pRefractionTexture != NULL)                 
        m_pd3device9->SetTexture(0, pRefractionTexture);  

    // Render our object!
    r = SendGeometryToVideoCard(pObj);

Error:

    pRefractionTexture->Release();
    pRefractionSurface->Release();
    pRefractionDepthStencil->Release();
    return r;
}

RESULT DX9ShaderManager::ShadePhongReflect( DX9DimObject *pObj, int depth, DimObject *pDX9ObjCaller,
                                            IDirect3DVertexShader9 *pVertexShader, ID3DXConstantTable *pVertexConstants, 
                                            IDirect3DPixelShader9 *pPixelShader, ID3DXConstantTable *pPixelConstants,
                                            DX9Camera *pRefractionCamera, DX9Camera *pReflectionCamera
                                          )
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;

    D3DXMATRIXA16 matWorldViewProj = GetWorldViewProjMat();

    if(depth >= DX9_SHADER_MAX_DEPTH)
        return R_EXIT;

    // First we need to decide on what kind of shader this material is
    DimObject *pDimObj = dynamic_cast<DimObject*>(pObj);    // need to acquire the DimObject Interface out of the DX9DimObject

    // Save back buffer and depth stencil for later
    IDirect3DSurface9 *pBackBuffer = NULL;      
    IDirect3DSurface9 *pDepthBuffer = NULL;
    m_pd3device9->GetRenderTarget(0, &pBackBuffer);
    m_pd3device9->GetDepthStencilSurface(&pDepthBuffer);

    IDirect3DTexture9 *pReflectionTexture = NULL;
    IDirect3DSurface9 *pReflectionSurface = NULL;
    IDirect3DSurface9 *pReflectionDepthStencil = NULL;
    plane *pReflectionPlane = NULL;

    // Initialize the reflection texture
    hr = m_pd3device9->CreateTexture(   PROJECTION_WIDTH,          // width
                                        PROJECTION_HEIGHT,         // height
                                        1,                         // levels (mipmaps etc)
                                        D3DUSAGE_RENDERTARGET,     // Usage (intentioned)
                                        D3DFMT_R5G6B5,             // Format 
                                        D3DPOOL_DEFAULT,           // Pool
                                        &pReflectionTexture,     // Texture Reference
                                        NULL                       // Handle (VISTA only boo)
                                        );

    CHRM(hr, "ShadePhongReflect: Could not create reflection texture");
    CHRM(pReflectionTexture->GetSurfaceLevel(0, &pReflectionSurface), "ShadePhongReflect: Couldn't get reflection surface level");   

    D3DSURFACE_DESC Desc;
    CHRM(pDepthBuffer->GetDesc(&Desc), "ShadePhongReflect: Failed to get depth stencil buffer description");

    hr = m_pd3device9->CreateDepthStencilSurface(   Desc.Width, 
                                                    Desc.Height, 
                                                    Desc.Format, 
                                                    Desc.MultiSampleType, 
                                                    Desc.MultiSampleQuality, 
                                                    false,
                                                    &pReflectionDepthStencil, 
                                                    NULL
                                                    );

    CHRM(hr, "ShadePhongReflect: Failed to create reflection depth stencil buffer");

    // Save a handle to the object store
    ObjectStore *pObjectStore = m_pDimImpDX9->GetObjectStore();                      

    // Set new Render target
    m_pd3device9->SetRenderTarget(0, pReflectionSurface);
    m_pd3device9->SetDepthStencilSurface(pReflectionDepthStencil);

    if(pReflectionTexture != NULL)
    {        
        // Lets create the reflection camera now
        // hard coded to a reflection plane in XZ with origin 0.0f 

        // Set up the clipping plane 
        pReflectionPlane = pDimObj->GetPlane();
        if(pReflectionPlane == NULL)            
            pReflectionPlane = new plane(point(0.0f, 0.0f, 0.0f), vector(0.0f, 1.0f, 0.0f), false, false);            
        vector ReflectNormal = pReflectionPlane->GetNormal();

        D3DXMATRIXA16 ProjMatrix;
        D3DXMATRIXA16 ViewMatrix;

        float CameraPlaneDistance;// = DistancePointPlane(pObjectStore->GetMainCamera()->GetOrigin(), pReflectionPlane);
        point CameraReflectionPoint;// = pObjectStore->GetMainCamera()->GetOrigin() - 2.0f * (pReflectionPlane->GetNormal() * CameraPlaneDistance);     

        // Fix this leak!!
        //pLocalReflectionCamera = new DX9Camera(CameraReflectionPoint, pObjectStore->GetMainCamera()->GetLookAtPosition(), 1.0f * pObjectStore->GetMainCamera()->GetUpDirection());
        //pLocalReflectionCamera->Render(m_pd3device9);
        //pReflectionCamera = pLocalReflectionCamera;   

        // The refraction camera will always be the view point of the main
        // camera with a different clipping plane
        if(pDX9ObjCaller != NULL)
        {
            if(pDX9ObjCaller->GetMaterialShadeStyle() == SHADE_PHONG_REFRACT && pRefractionCamera != NULL)
            {               
                CameraPlaneDistance = DistancePointPlane(pRefractionCamera->GetOrigin(), pReflectionPlane);
                CameraReflectionPoint = pRefractionCamera->GetOrigin() - 2.0f * (pReflectionPlane->GetNormal() * CameraPlaneDistance);
                
                pRefractionCamera->Render(m_pd3device9);
                ProjMatrix = pRefractionCamera->GetProjectionMatrix();
                ViewMatrix = pRefractionCamera->GetViewMatrix();
            }
            else if(pDX9ObjCaller->GetMaterialShadeStyle() == SHADE_PHONG_REFLECT && pReflectionCamera != NULL)
            {
                CameraPlaneDistance = DistancePointPlane(pReflectionCamera->GetOrigin(), pReflectionPlane);
                CameraReflectionPoint = pReflectionCamera->GetOrigin() - 2.0f * (pReflectionPlane->GetNormal() * CameraPlaneDistance);
                
                pReflectionCamera->Render(m_pd3device9);
                ProjMatrix = pReflectionCamera->GetProjectionMatrix();
                ViewMatrix = pReflectionCamera->GetViewMatrix();
                ReflectNormal = ReflectNormal * -1.0f;  // flip the clipping plane
            }
        }
        else
        {
            CameraPlaneDistance = DistancePointPlane(pObjectStore->GetMainCamera()->GetOrigin(), pReflectionPlane);
            CameraReflectionPoint = pObjectStore->GetMainCamera()->GetOrigin() - 2.0f * (pReflectionPlane->GetNormal() * CameraPlaneDistance);  
            
            pReflectionCamera = new DX9Camera(CameraReflectionPoint, pObjectStore->GetMainCamera()->GetLookAtPosition(), 1.0f * pObjectStore->GetMainCamera()->GetUpDirection());
            pReflectionCamera->Render(m_pd3device9);
            pReflectionCamera = pReflectionCamera; 

            ProjMatrix = pReflectionCamera->GetProjectionMatrix();
            ViewMatrix = pReflectionCamera->GetViewMatrix();
        }

        D3DXPLANE clipPlane;
        D3DXPlaneFromPointNormal(&clipPlane, &D3DXVECTOR3(pXYZ(pReflectionPlane->m_ptOrigin)), &D3DXVECTOR3(vXYZ(ReflectNormal)));      

        //D3DXMATRIXA16 ProjMatrix = pLocalReflectionCamera->GetProjectionMatrix();
        //D3DXMATRIXA16 ViewMatrix = pLocalReflectionCamera->GetViewMatrix();
                
        D3DXMATRIXA16 WorldViewMat = ViewMatrix * ProjMatrix;
        D3DXMATRIXA16 trueClipPlane = WorldViewMat;
        D3DXMatrixInverse(&trueClipPlane, NULL, &trueClipPlane);
        D3DXMatrixTranspose(&trueClipPlane, &trueClipPlane);
        D3DXPLANE viewSpacePlane;
        D3DXPlaneTransform(&viewSpacePlane, &clipPlane, &trueClipPlane);
        m_pd3device9->SetClipPlane(0, (float *)viewSpacePlane);
        m_pd3device9->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

        // Set new Render target
        m_pd3device9->SetRenderTarget(0, pReflectionSurface);

        m_pd3device9->Clear(0,
            NULL,
            D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
            D3DCOLOR_XRGB(100, 100, 100),
            1.0f,
            0);
        // Draw the scene                  
        m_pd3device9->BeginScene();
        {
            // Render the rest of the scene incrementing the depth and avoiding self reflection by passing
            // the object as the caller.  So when the render function is reached for this object it will be skipped
            pObjectStore->GetDimensionObjectDataBase()->Iterate((kdtree::FunctionPtrArgDepthObjCams)RenderDimObject, m_pDimImpDX9, depth + 1, pDimObj, NULL, pReflectionCamera);
        }
        m_pd3device9->EndScene(); 

        // Turn off the clipping plane
        m_pd3device9->SetRenderState( D3DRS_CLIPPLANEENABLE, 0x00 );

        if(pDimObj->GetPlane() == NULL)
        {
            delete pReflectionPlane;
            pReflectionPlane = NULL;
        }
    }
    // Now we reset the back buffer and the depth stencil        
    m_pd3device9->SetRenderTarget(0, pBackBuffer);  
    m_pd3device9->SetDepthStencilSurface(pDepthBuffer);

    // release to avoid memory leak
    pDepthBuffer->Release();
    pBackBuffer->Release();

    // reset camera 
    //reinterpret_cast<DX9Camera*>(pObjectStore->GetMainCamera())->Render(m_pd3device9);
    if(pDX9ObjCaller != NULL)
    {
        if(pDX9ObjCaller->GetMaterialShadeStyle() == SHADE_PHONG_REFRACT && pRefractionCamera != NULL)
        {
            pRefractionCamera->Render(m_pd3device9);
        }
        else if(pDX9ObjCaller->GetMaterialShadeStyle() == SHADE_PHONG_REFLECT && pReflectionCamera != NULL)
        {
            pReflectionCamera->Render(m_pd3device9);
        }
    }
    else
    {
        //pReflectionCamera->Render(m_pd3device9);
        reinterpret_cast<DX9Camera*>(pObjectStore->GetMainCamera())->Render(m_pd3device9);
    }

    // Pass the World View Projection Matrix and object transformation matrices to the Vertex Shader
    pVertexConstants->SetMatrix(m_pd3device9, "g_WorldViewProj", &matWorldViewProj); 
    pVertexConstants->SetMatrix(m_pd3device9, "g_Transform", &(pObj->m_matTransform));

    // FAKE THE LIGHTING for now
    static float theta = 0.0f;
    //theta -= .005;
    float radius = 2.5f;
    float LightXVal = radius * sin(theta);
    LightXVal = 2.5f;
    float LightZVal = radius * cos(theta);

    // NEED TO CALL lighting/camera manager here in reality
    pVertexConstants->SetFloatArray(m_pd3device9, "g_ViewPosition", point(0.0f, 5.0f, 0.0f).tuple, 3);
    pVertexConstants->SetFloatArray(m_pd3device9, "g_light0", point(LightXVal, 2.5f, LightZVal).tuple, 3);

    // Set up some reflection specific details (need the implementation)
    pPixelConstants->SetBool(m_pd3device9, "g_Reflection", true);

    //D3DXMATRIXA16 ReflectionCamView = m_pDimImpDX9->m_ReflectionCamera.GetViewMatrix();
    //D3DXMATRIXA16 ReflectionCamView = static_cast<DX9Material*>(pDimObj->m_pMaterial)->m_pReflectionCamera->GetViewMatrix();            
    
    //D3DXMATRIXA16 ReflectionCamView = pLocalReflectionCamera->GetViewMatrix();            
    //D3DXMATRIXA16 OriginalCamProjection = reinterpret_cast<DX9Camera*>(m_pDimImpDX9->GetObjectStore()->GetMainCamera())->GetProjectionMatrix();

    D3DXMATRIXA16 ReflectionCamView;            
    D3DXMATRIXA16 OriginalCamProjection;
    
    D3DXMATRIXA16 TrafoMatrix(  0.5f,  0.0f, 0.0f, 0.0f, 
                                0.0f, -0.5f, 0.0f, 0.0f,
                                0.0f,  0.0f, 0.0f, 0.0f, 
                                0.5f,  0.5f, 0.0f, 1.0f );

    if(pDX9ObjCaller != NULL)
    {
        if(pRefractionCamera != NULL)
        {
            ReflectionCamView = pRefractionCamera->GetViewMatrix();
            OriginalCamProjection = pRefractionCamera->GetProjectionMatrix();
        }
        else if(pReflectionCamera != NULL)
        {
            ReflectionCamView = pReflectionCamera->GetViewMatrix();
            OriginalCamProjection = pReflectionCamera->GetProjectionMatrix();
        }
    }
    else
    {
        ReflectionCamView = pReflectionCamera->GetViewMatrix();
        OriginalCamProjection = pReflectionCamera->GetProjectionMatrix();
    }

    D3DXMATRIXA16 ReflectionProjectionMatrix = ReflectionCamView * OriginalCamProjection * TrafoMatrix;
    pVertexConstants->SetMatrix(m_pd3device9, "g_ReflectionProjectionMatrix", &ReflectionProjectionMatrix);

    // Pass the shaders to the device
    m_pd3device9->SetVertexShader(pVertexShader);
    m_pd3device9->SetPixelShader(pPixelShader);

    // Pass the diffuse, bump textures to the device
    CRM(static_cast<DX9Material*>(pDimObj->m_pMaterial)->SetTextures(m_pd3device9, TEXTURE_DIFFUSE ), "DX9ShaderManager: Failed to Set Diffuse Textures for Rendering");
    CRM(static_cast<DX9Material*>(pDimObj->m_pMaterial)->SetTextures(m_pd3device9, TEXTURE_BUMP), "DX9ShaderManager: Failed to Set Bump Textures for Rendering");

    // Reflect Textures
    //if(static_cast<DX9Material*>(pDimObj->m_pMaterial)->m_pReflectionTexture != NULL)                 
    //    m_pd3device9->SetTexture(0, static_cast<DX9Material*>(pDimObj->m_pMaterial)->m_pReflectionTexture); 

    if(pReflectionTexture != NULL)                 
        m_pd3device9->SetTexture(0, pReflectionTexture); 

    // Render our object!
    r = SendGeometryToVideoCard(pObj);

Error:
    /*if(pLocalReflectionCamera != NULL)
    {
        delete pLocalReflectionCamera;
        pLocalReflectionCamera = NULL;
    }*/

    pReflectionTexture->Release();
    pReflectionSurface->Release();
    pReflectionDepthStencil->Release();
    return r;
}

RESULT DX9ShaderManager::ShadePhong( DX9DimObject *pObj, int depth, DimObject *pDX9ObjCaller,
                                     IDirect3DVertexShader9 *pVertexShader, ID3DXConstantTable *pVertexConstants, 
                                     IDirect3DPixelShader9 *pPixelShader, ID3DXConstantTable *pPixelConstants
                                   )
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;

    D3DXMATRIXA16 matWorldViewProj = GetWorldViewProjMat();

    // First we need to decide on what kind of shader this material is
    DimObject *pDimObj = dynamic_cast<DimObject*>(pObj);    // need to acquire the DimObject Interface out of the DX9DimObject

    // Pass the World View Projection Matrix and object transformation matrices to the Vertex Shader
    pVertexConstants->SetMatrix(m_pd3device9, "g_WorldViewProj", &matWorldViewProj); 
    pVertexConstants->SetMatrix(m_pd3device9, "g_Transform", &(pObj->m_matTransform));

    // FAKE THE LIGHTING for now
    static float theta = 0.0f;
    //theta -= .005;
    float radius = 2.5f;
    float LightXVal = radius * sin(theta);
    LightXVal = 2.5f;
    float LightZVal = radius * cos(theta);

    // NEED TO CALL lighting/camera manager here in reality
    pVertexConstants->SetFloatArray(m_pd3device9, "g_ViewPosition", point(0.0f, 5.0f, 0.0f).tuple, 3);
    pVertexConstants->SetFloatArray(m_pd3device9, "g_light0", point(LightXVal, 2.5f, LightZVal).tuple, 3);

    static float mult = 0.0f;
    mult += .00025;
    pPixelConstants->SetFloat(m_pd3device9, "g_Multiplier", abs(sin(mult)));

    // Pass the shaders to the device
    m_pd3device9->SetVertexShader(pVertexShader);
    m_pd3device9->SetPixelShader(pPixelShader);

    // Pass the diffuse, bump textures to the device
    CRM(static_cast<DX9Material*>(pDimObj->m_pMaterial)->SetTextures(m_pd3device9, TEXTURE_DIFFUSE ), "DX9ShaderManager: Failed to Set Diffuse Textures for Rendering");
    CRM(static_cast<DX9Material*>(pDimObj->m_pMaterial)->SetTextures(m_pd3device9, TEXTURE_BUMP ), "DX9ShaderManager: Failed to Set Bump Textures for Rendering");

    // Render our object!
    r = SendGeometryToVideoCard(pObj);

Error:
    return r;
}

RESULT DX9ShaderManager::ShadeFlat( DX9DimObject *pObj, int depth, DimObject *pDX9ObjCaller,
                                    IDirect3DVertexShader9 *pVertexShader, ID3DXConstantTable *pVertexConstants, 
                                    IDirect3DPixelShader9 *pPixelShader, ID3DXConstantTable *pPixelConstants
                                    )
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;

    D3DXMATRIXA16 matWorldViewProj = GetWorldViewProjMat();

    // First we need to decide on what kind of shader this material is
    DimObject *pDimObj = dynamic_cast<DimObject*>(pObj);    // need to acquire the DimObject Interface out of the DX9DimObject

    // Pass the World View Projection Matrix and object transformation matrices to the Vertex Shader
    pVertexConstants->SetMatrix(m_pd3device9, "g_WorldViewProj", &matWorldViewProj); 
    pVertexConstants->SetMatrix(m_pd3device9, "g_Transform", &(pObj->m_matTransform));

    // Pass to the shaders to the device
    m_pd3device9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); 
    m_pd3device9->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pd3device9->SetVertexShader(pVertexShader);
    m_pd3device9->SetPixelShader(pPixelShader);

    // Pass the textures to the device
    CRM(static_cast<DX9Material*>(pDimObj->m_pMaterial)->SetTextures(m_pd3device9, TEXTURE_DIFFUSE), "DX9Shpere: Failed to Set Textures for Rendering");

    // Render our object!
    r = SendGeometryToVideoCard(pObj);

Error:
    return r;
}

D3DXMATRIXA16 DX9ShaderManager::GetWorldViewProjMat()
{
    // All shaders use this information
    D3DXMATRIXA16 matWorld, matView, matProj;
    m_pd3device9->GetTransform(D3DTS_WORLD, &matWorld);
    m_pd3device9->GetTransform(D3DTS_VIEW, &matView);
    m_pd3device9->GetTransform(D3DTS_PROJECTION, &matProj);
    return matWorld * matView * matProj;
}

RESULT DX9ShaderManager::SetupDX9DeviceForObject( DX9DimObject *pObj, int depth, DimObject *pDX9ObjCaller, DX9Camera *pRefractionCamera, DX9Camera *pReflectionCamera)
{
    RESULT r = R_OK;
    HRESULT hr = S_OK;

    // First we need to decide on what kind of shader this material is
    DimObject *pDimObj = dynamic_cast<DimObject*>(pObj);    // need to acquire the DimObject Interface out of the DX9DimObject

    // Shouldn't render certain object types
    // They still get updated but not sent to render
    DIM_OBJECT_TYPE dot = pDimObj->GetObjectType();
    switch(dot)
    {
        case DIM_OBJECT:
        case DIM_LIGHT: 
        case DIM_CAMERA:    
        case DIM_INVALID:   return R_EXIT;
                            break;
    }        

    SHADE_STYLE pss = pDimObj->GetMaterialShadeStyle();

    if(depth >= DX9_SHADER_MAX_DEPTH && (pss == SHADE_PHONG_REFRACT || pss == SHADE_PHONG_REFLECT) )
    {
        return R_EXIT;
    }  

    // test a theory
    ///*
    if(pDX9ObjCaller != NULL && pDimObj->GetPlane() != NULL)     
        if(pDX9ObjCaller->GetMaterialShadeStyle() == SHADE_PHONG_REFLECT)
            if(pY(pDimObj->GetPlane()->m_ptOrigin) < pY(pDX9ObjCaller->GetPlane()->m_ptOrigin))
                return R_EXIT;
        else if(pDX9ObjCaller->GetMaterialShadeStyle() == SHADE_PHONG_REFRACT)
            if(pY(pDimObj->GetPlane()->m_ptOrigin) > pY(pDX9ObjCaller->GetPlane()->m_ptOrigin))
                return R_EXIT;            
    //*/
    
    m_pd3device9->SetVertexDeclaration(m_pD3DVertexDeclaration);        // for now the same vertex decleration is used for all objects

    // Set up the World Transform 
    m_pd3device9->SetTransform(D3DTS_WORLD, &(pObj->m_matTransform));

    // Acquire the appropriate shader
    IDirect3DVertexShader9 *pVertexShader = m_pVertexShaders[(DX9_SHADER)pss];
    ID3DXConstantTable *pVertexConstants = m_pVertexConstants[(DX9_SHADER)pss];
    IDirect3DPixelShader9 *pPixelShader = m_pPixelShaders[(DX9_SHADER)pss];
    ID3DXConstantTable *pPixelConstants = m_pPixelConstants[(DX9_SHADER)pss];

    switch(pss)
    {
        case SHADE_FLAT:
        {
            if(ShadeFlat( pObj, depth, pDX9ObjCaller, pVertexShader, pVertexConstants, pPixelShader, pPixelConstants) == R_EXIT)
                return R_EXIT;

        } break;

        case SHADE_PHONG:
        {            
            if(ShadePhong( pObj, depth, pDX9ObjCaller, pVertexShader, pVertexConstants, pPixelShader, pPixelConstants) == R_EXIT)
                return R_EXIT;            
        } break;

        case SHADE_PHONG_REFRACT:
        {                                                            
            if(ShadePhongRefract( pObj, depth, pDX9ObjCaller, pVertexShader, pVertexConstants, pPixelShader, pPixelConstants, pRefractionCamera, pReflectionCamera) == R_EXIT)
                return R_EXIT;
        } break;

        case SHADE_PHONG_REFLECT:
        {                        
            if(ShadePhongReflect( pObj, depth, pDX9ObjCaller, pVertexShader, pVertexConstants, pPixelShader, pPixelConstants, pRefractionCamera, pReflectionCamera) == R_EXIT)
                return R_EXIT;
        } break;

        default:    
        {
            printf("DX9ShaderManager:Unsupported shade style!");
        } break;
    } // end of switch    

Error:
    return r;
}

RESULT DX9ShaderManager::SendGeometryToVideoCard(DX9DimObject *pObj)
{
    RESULT r = R_OK;
    
    // first try the better one (the new one) eventually migrate it all over
    //r = dynamic_cast<DX9DimObject*>(pDX9Obj)->Render((DimensionImpDX9*)(pDimImpDX9));
    r = pObj->Render(m_pDimImpDX9);

    if(r == R_NOT_IMPLEMENTED)
    {
        if(m_pd3device9 != NULL)            
            r = pObj->Render(m_pd3device9);        
        else        
            printf("Device is NULL!");        
    }

Error:
    return r;
}

DX9ShaderManager::DX9ShaderManager( DimensionImpDX9 *pDimImpDX9 ) :
    m_pDimImpDX9(pDimImpDX9)
{
    HRESULT hr = S_OK;

    m_pd3device9 = m_pDimImpDX9->GetDirectX9Device();

    // Set up the primary vertex declaration
    const D3DVERTEXELEMENT9 tempVertexDecl[] = 
    {
        {0,     0,      D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0},
        {0,     12,     D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,    0},
        {0,     24,     D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TANGENT,    0},
        {0,     36,     D3DDECLTYPE_FLOAT4,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR,     0},
        {0,     52,     D3DDECLTYPE_FLOAT2,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0},
        D3DDECL_END()
    };    
    hr = m_pd3device9->CreateVertexDeclaration(tempVertexDecl, &m_pD3DVertexDeclaration); 
    if(FAILED(hr))    
        printf("DimensionImpDX9::Could not create primary vertex declaration!\n"); 

    // Allocate the Shader Array
    m_pVertexShaders = new IDirect3DVertexShader9*[DX9_SHADER_COUNT];
    m_pVertexConstants = new ID3DXConstantTable*[DX9_SHADER_COUNT];
    m_pPixelShaders = new IDirect3DPixelShader9*[DX9_SHADER_COUNT];
    m_pPixelConstants = new ID3DXConstantTable*[DX9_SHADER_COUNT];

    InitializeShaders();
}

#endif // #ifdef __WIN32__
