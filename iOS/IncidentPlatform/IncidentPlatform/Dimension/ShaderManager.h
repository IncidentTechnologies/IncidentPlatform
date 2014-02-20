#pragma once

// This is a place holder for the ShaderManager which is just an abstract interface.
// This should provide quality assurance as new ShaderManagers are set up for 
// different platforms

#include "RESULT.h"
#include "EHM.h"
#include "material.h"

typedef enum SupportedShaders
{
    SHADER_FLAT                     =   SHADE_FLAT,
    SHADER_GOURARD                  =   SHADE_GOURARD,
    SHADER_PHONG                    =   SHADE_PHONG,
    SHADER_PHONG_REFLECT            =   SHADE_PHONG_REFLECT,
    SHADER_PHONG_REFRACT            =   SHADE_PHONG_REFRACT,
    SHADER_PHONG_REFLECT_REFRACT    =   SHADE_PHONG_REFLECT_REFRACT ,
    SHADER_INVALID
} SHADER;

class ShaderManager
{
public:
    // Shader Managers are required to implement these shaders
    RESULT InitializeShaders()
    {
        RESULT r = R_OK;        
        r = InitializeFlatShader();
        r = InitializeGourardShader();
        r = InitializePhongShader();
        r = InitializePhongReflectShader();
        r = InitializePhongRefractShader();
        r = InitializePhongReflectRefractShader();

Error:
        return r;
    }

    virtual RESULT InitializeFlatShader() = 0;
    virtual RESULT InitializeGourardShader() = 0;
    virtual RESULT InitializePhongShader() = 0;
    virtual RESULT InitializePhongReflectShader() = 0;
    virtual RESULT InitializePhongRefractShader() = 0;
    virtual RESULT InitializePhongReflectRefractShader() = 0;

};