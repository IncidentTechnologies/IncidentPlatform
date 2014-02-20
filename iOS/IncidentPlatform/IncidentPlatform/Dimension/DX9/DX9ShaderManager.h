#ifdef __WIN32__
#pragma once

#include "ShaderManager.h"

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

// The DirectX9 Shader Manager effectively sits as a module to aid in setting up the appropriate 
// shader for a given material.  For example, if an object is shiny and has 3 textures the 
// shader manager will set up the DirectX9 device to render

// The shader manager also takes care of vertex declerations since different shaders will require
// different shaders as well as passing the lighting information to these shaders.  Effectively the
// shader manager is a bottle neck through which lighting/material/texture information is all passed
// into the actual device

#include "DX9Primitives.h"

class DimensionImpDX9;

typedef enum DX9SupportedShaders
{
    DX9_SHADER_FLAT                     = SHADER_FLAT,
    DX9_SHADER_GOURARD                  = SHADER_GOURARD,
    DX9_SHADER_PHONG                    = SHADER_PHONG,
    DX9_SHADER_PHONG_REFLECT            = SHADER_PHONG_REFLECT,
    DX9_SHADER_PHONG_REFRACT            = SHADER_PHONG_REFRACT,
    DX9_SHADER_PHONG_REFLECT_REFRACT    = SHADER_PHONG_REFLECT_REFRACT,
    DX9_SHADER_INVALID                  = SHADER_INVALID
} DX9_SHADER;

#define DX9_SHADER_COUNT 6

// Lets set our depth here this determines how deep we allow the shader engine
// to go in regards to refractions and reflections
#define DX9_SHADER_MAX_DEPTH 3

// This class should not have any reference to the actual 
class DX9ShaderManager : 
    public ShaderManager
{
public:
    DX9ShaderManager(DimensionImpDX9 *pDimImpDX9);

    ~DX9ShaderManager()
    {

    }

    D3DXMATRIXA16 DX9ShaderManager::GetWorldViewProjMat();

    RESULT SendGeometryToVideoCard(DX9DimObject *pObj);

    RESULT SetupDX9DeviceForObject(DX9DimObject *pObj, int depth, DimObject *pDX9ObjCaller, DX9Camera *pRefractionCamera, DX9Camera *pReflectionCamera);
    RESULT ShadePhong(  DX9DimObject *pObj, int depth, DimObject *pDX9ObjCaller, 
                        IDirect3DVertexShader9 *pVertexShader, ID3DXConstantTable *pVertexConstants, 
                        IDirect3DPixelShader9 *pPixelShader, ID3DXConstantTable *pPixelConstants
                     );

    RESULT ShadeFlat(   DX9DimObject *pObj, int depth, DimObject *pDX9ObjCaller, 
                        IDirect3DVertexShader9 *pVertexShader, ID3DXConstantTable *pVertexConstants, 
                        IDirect3DPixelShader9 *pPixelShader, ID3DXConstantTable *pPixelConstants
                    );

    RESULT ShadePhongRefract(   DX9DimObject *pObj, int depth, DimObject *pDX9ObjCaller, 
                                IDirect3DVertexShader9 *pVertexShader, ID3DXConstantTable *pVertexConstants, 
                                IDirect3DPixelShader9 *pPixelShader, ID3DXConstantTable *pPixelConstants,
                                DX9Camera *pRefractionCamera, DX9Camera *pReflectionCamera
                            );
    
    RESULT ShadePhongReflect(   DX9DimObject *pObj, int depth, DimObject *pDX9ObjCaller, 
                                IDirect3DVertexShader9 *pVertexShader, ID3DXConstantTable *pVertexConstants, 
                                IDirect3DPixelShader9 *pPixelShader, ID3DXConstantTable *pPixelConstants,
                                DX9Camera *pRefractionCamera, DX9Camera *pReflectionCamera
                            );

    RESULT InitializeVertexShader(LPCWSTR lpcwstrPixelShaderFilename, DX9_SHADER shaderType)
    {
        RESULT r = R_OK;
        HRESULT hr = S_OK;

        char *pszVertexShaderProfile = (char*)(D3DXGetVertexShaderProfile(m_pd3device9));
        
        printf(" Vertex Shader: %s ", pszVertexShaderProfile);

        // Set up the Vertex Shader
        hr = D3DXCompileShaderFromFile( lpcwstrPixelShaderFilename,      // shader file
                                        NULL,                        // Macros
                                        NULL,                        // includes
                                        "vs_main",                   // main function
                                        pszVertexShaderProfile,      // shader profile
                                        0,                           // flags
                                        &m_pTempBuffer,              // compiled operations
                                        NULL,                        // errors
                                        &(m_pVertexConstants[shaderType])      // constants
                                        );
        CHRM(hr, "DX9ShaderManager::failed to create vertex shader %d", shaderType);

        CHRM(m_pd3device9->CreateVertexShader((DWORD*)m_pTempBuffer->GetBufferPointer(), &(m_pVertexShaders[shaderType])), 
            "DX9ShaderManager::Could not create the vertex shader %d", shaderType);

        m_pTempBuffer->Release();

Error:
        return r;
    }

    RESULT InitializePixelShader(LPCWSTR lpcwstrPixelShaderFilename, DX9_SHADER shaderType)
    {
        RESULT r = R_OK;
        HRESULT hr = S_OK;

        char *pszPixelShaderProfile = (char*)(D3DXGetPixelShaderProfile(m_pd3device9));

        printf("Pixel Shader: %s\n", pszPixelShaderProfile);       

        // Set up the pixel shader
        hr = D3DXCompileShaderFromFile( lpcwstrPixelShaderFilename,     // shader filename
                                        NULL,                           // Macros
                                        NULL,                           // includes
                                        "ps_main",                      // main function
                                        pszPixelShaderProfile,          // shader profile
                                        0,                              // flags
                                        &m_pTempBuffer,                 // compiled operations
                                        NULL,                           // errors
                                        &(m_pPixelConstants[shaderType])                // constants  
                                        ); 
        CHRM(hr, "DX9ShaderManager::failed to create pixel shader %d", shaderType);

        CHRM(m_pd3device9->CreatePixelShader((DWORD*)m_pTempBuffer->GetBufferPointer(), &(m_pPixelShaders[shaderType])),
            "DX9ShaderManager::Could not create the pixel shader %d", shaderType);

        m_pTempBuffer->Release();

Error:
        return r;
    }

    IDirect3DVertexShader9 *GetVertexShader(DX9_SHADER shaderType){ return m_pVertexShaders[shaderType]; }
    ID3DXConstantTable *GetVertexConstants(DX9_SHADER shaderType){ return m_pVertexConstants[shaderType]; }
    IDirect3DPixelShader9 *GetPixelShader(DX9_SHADER shaderType){ return m_pPixelShaders[shaderType]; }
    ID3DXConstantTable *GetPixelConstants(DX9_SHADER shaderType){ return m_pPixelConstants[shaderType]; }

    RESULT InitializeFlatShader()
    {
        RESULT r = R_OK;
        printf("Flat Shading using" );
        CRM(InitializeVertexShader(L"./shader/Unshaded/vertex.vsh", DX9_SHADER_FLAT), "DX9ShaderManager::Failed to initialze flat vertex shader");
        CRM(InitializePixelShader(L"./shader/Unshaded/pixel.psh", DX9_SHADER_FLAT), "DX9ShaderManager::Failed to initialize flat pixel shader");
Error:
        return r;
    }


    RESULT InitializeGourardShader()
    {
        RESULT r = R_NOT_IMPLEMENTED;

Error:
        return r;
    }


    RESULT InitializePhongShader()
    {
        RESULT r = R_OK;       
        printf("Phong Shading using" );
        CRM(InitializeVertexShader(L"./shader/bump/vertex.vsh", DX9_SHADER_PHONG), "DX9ShaderManager::Failed to initialze phong vertex shader");
        CRM(InitializePixelShader(L"./shader/bump/pixel.psh", DX9_SHADER_PHONG), "DX9ShaderManager::Failed to initialize phong pixel shader");
Error:
        return r;
    }

    RESULT InitializePhongReflectShader()
    {
        RESULT r = R_OK;
        printf("Phong Reflect Shading using" );
        CRM(InitializeVertexShader(L"./shader/BumpReflect/vertex.vsh", DX9_SHADER_PHONG_REFLECT), "DX9ShaderManager::Failed to initialze phong reflect vertex shader");
        CRM(InitializePixelShader(L"./shader/BumpReflect/pixel.psh", DX9_SHADER_PHONG_REFLECT), "DX9ShaderManager::Failed to initialize phong reflect pixel shader");
        //CRM(InitializeVertexShader(L"./shader/BumpRefract/vertex.vsh", DX9_SHADER_PHONG_REFLECT), "DX9ShaderManager::Failed to initialze phong reflect vertex shader");
        //CRM(InitializePixelShader(L"./shader/BumpRefract/pixel.psh", DX9_SHADER_PHONG_REFLECT), "DX9ShaderManager::Failed to initialize phong reflect pixel shader");
Error:
        return r;
    }

    RESULT InitializePhongRefractShader()
    {
        RESULT r = R_OK;
        printf("Phong Refract Shading using" );
        CRM(InitializeVertexShader(L"./shader/BumpRefract/vertex.vsh", DX9_SHADER_PHONG_REFRACT), "DX9ShaderManager::Failed to initialze phong refract vertex shader");
        CRM(InitializePixelShader(L"./shader/BumpRefract/pixel.psh", DX9_SHADER_PHONG_REFRACT), "DX9ShaderManager::Failed to initialize phong refract pixel shader");
Error:
        return r;
    }

    RESULT InitializePhongReflectRefractShader()
    {
        RESULT r = R_NOT_IMPLEMENTED;

Error:
        return r;
    }

public:
    // Moving shaders and vertex decelerations out of the objects and into the implementation
    // since we are passing the DimentionImplementation object to the object for rendering
    // this aids in initializing as well as reuse of the objects (memory!)    
    IDirect3DVertexDeclaration9 *m_pD3DVertexDeclaration;

    // SHADERS
    ID3DXBuffer *m_pTempBuffer;

    IDirect3DVertexShader9  **m_pVertexShaders;
    ID3DXConstantTable      **m_pVertexConstants;
    IDirect3DPixelShader9   **m_pPixelShaders;
    ID3DXConstantTable      **m_pPixelConstants;

private:
    PDIRECT3DDEVICE9 m_pd3device9;  // handle of the directx9 device
    DimensionImpDX9 *m_pDimImpDX9;

public:
    

};

#endif // #ifdef __WIN32__
