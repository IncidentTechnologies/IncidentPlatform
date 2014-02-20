#pragma once

// Material is a class which encapsulates all the different possible 
// material properties of certain material.  The material will define
// the different properties of a material as well as manage it's textures.
// This will in turn be used by the engine to correctly render the material.
// Also it will be emphasized to allow for custom shader support as well

#include "color.h"
#include "texture.h"

typedef enum ShadeStyle
{
    SHADE_FLAT,                     // lighting computed for each face
    SHADE_GOURARD,                  // lighting computed for each vertex
    SHADE_PHONG,                    // lighting computer for each pixel
    SHADE_PHONG_REFLECT,            // lighting computer for each pixel and reflection
    SHADE_PHONG_REFRACT,            // lighting computer for each pixel and refraction
    SHADE_PHONG_REFLECT_REFRACT,    // lighting computer for each pixel, reflection and refraction
    SHADE_INVALID                   // not a valid shading style
} SHADE_STYLE;

typedef enum TextureType
{
    TEXTURE_AMBIENT         ,
    TEXTURE_DIFFUSE         ,
    TEXTURE_BUMP            ,
    TEXTURE_SPECULAR        ,
    TEXTURE_SPECULAR_SHINY  ,
    TEXTURE_REFLECT         ,
    TEXTURE_REFRACT         ,
    TEXTURE_TRANSPARENT     ,
    TEXTURE_INVALID         
} TEXTURE_TYPE;

#define TEXTURE_COUNT TEXTURE_INVALID  

typedef enum MaterialParameter
{
    MATERIAL_SPECULAR_SHINE,
    MATERIAL_REFLECT,
    MATERIAL_REFRACT,
    MATERIAL_TRANSPARENT,
    MATERIAL_SHADE_STYLE,
    MATERIAL_INVALID
} MATERIAL_PARAM;

#define MATERIAL_PARAM_COUNT MATERIAL_INVALID

class material
{
public:
    material() :
      m_textures(NULL),
      m_SpecularShinyness(0.0f),
      m_Reflectivity(0.0f),
      m_Refractivity(0.0f),
      m_Transparency(0.0f),
      m_ShadeStyle(SHADE_FLAT)
    {
        m_textures = new texture[TEXTURE_COUNT];
    }

    material(material *pMaterial) :
        m_textures(NULL),
        m_SpecularShinyness(0.0f),
        m_Reflectivity(0.0f),
        m_Refractivity(0.0f),
        m_Transparency(0.0f),
        m_ShadeStyle(SHADE_FLAT)
    {
        m_textures = new texture[TEXTURE_COUNT];
        memcpy(m_textures, pMaterial->m_textures, (sizeof(texture) * TEXTURE_COUNT));
    }

    ~material()
    {
        if(m_textures != NULL)
        {        
            //delete [] m_textures;
            //m_textures = NULL;
        }
    }

    texture GetTexture(TEXTURE_TYPE tt)
    {
        return m_textures[tt];
    }

    RESULT AddTexture(char *pszFilename, TEXTURE_TYPE tt)
    {
        RESULT r = R_SUCCESS;

        //CBRM((tt < TEXTURE_INVALID), "Trying to add invalid texture %d file %s", tt, pszFilename);
        CR(m_textures[tt].AddTexture(pszFilename));

Error:
        return r;
    }

    int GetTextureCount(TEXTURE_TYPE tt)
    { 
        return m_textures[tt].GetTextureCount(); 
    }    

    RESULT GetMaterialParameter(MATERIAL_PARAM mp, void* &r_pValue)
    {
        RESULT r = R_SUCCESS;

        switch(mp)
        {
            case MATERIAL_SPECULAR_SHINE:   
            {
                r_pValue = (void*)(new float(m_SpecularShinyness));                
            } break;

            case MATERIAL_REFLECT:   
            {
                r_pValue = (void*)(new float(m_Reflectivity));                
            } break;

            case MATERIAL_REFRACT:   
            {
                r_pValue = (void*)(new float(m_Refractivity));                
            } break;

            case MATERIAL_TRANSPARENT:   
            {
                r_pValue = (void*)(new float(m_Transparency));                
            } break;

            case MATERIAL_SHADE_STYLE:
            {
                r_pValue = (void*)(new SHADE_STYLE(m_ShadeStyle)); 
            } break;
        }

Error:
        return r;
    }


    RESULT SetMaterialParameter(MATERIAL_PARAM mp, void *pValue)
    {
        RESULT r = R_SUCCESS;

        switch(mp)
        {
            case MATERIAL_SPECULAR_SHINE:   
            {
                m_SpecularShinyness = *(static_cast<float*>(pValue));          
            } break;

            case MATERIAL_REFLECT:   
            {
                m_Reflectivity = *(static_cast<float*>(pValue));             
            } break;

            case MATERIAL_REFRACT:   
            {
                m_Refractivity = *(static_cast<float*>(pValue)); 
            } break;

            case MATERIAL_TRANSPARENT:   
            {
                m_Transparency = *(static_cast<float*>(pValue)); 
            } break;

            case MATERIAL_SHADE_STYLE:
            {
                m_ShadeStyle = *(static_cast<SHADE_STYLE*>(pValue));
            } break;
        }

Error:
        return r;
    }

public:
    // Textures
    texture *m_textures;

    // These determine which shader is loaded by the ShaderManager
    float m_SpecularShinyness;
    float m_Reflectivity;
    float m_Refractivity;
    float m_Transparency;

    SHADE_STYLE m_ShadeStyle;
};