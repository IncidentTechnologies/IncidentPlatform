#ifdef __WIN32__
#pragma once

// This is the DirectX9 implementation specific material class

#include "material.h"
#include "DX9CmnPriv.h"
#include "DX9Camera.h"

#define PROJECTION_WIDTH 512
#define PROJECTION_HEIGHT 512

class DX9Material : 
    public material
{
public:
    DX9Material(material *pMaterial) :
      material(pMaterial),
      m_pppTextures(NULL)
      
      /*m_pReflectionCamera(NULL),
      m_Reflection_f(false),

      m_pRefractionCamera(NULL),
      m_Refraction_f(false)*/
    {
        
    }

    ~DX9Material()
    {
        // Properly release everything
        // Properly release the textures one by one
        for(int i = 0; i < TEXTURE_COUNT; i++)
        {
            for(int j = 0; j < GetTextureCount((TEXTURE_TYPE)i); j++)
            {
                if(m_pppTextures[i][j] != NULL)
                {
                    m_pppTextures[i][j]->Release();
                }
            }
        }
    }

    RESULT Initialize(LPDIRECT3DDEVICE9 pd3device9)
    {
        RESULT r = R_OK;

        CRM(InitializeTextures(pd3device9), "DX9Material: Failed to initialize textures");

Error:
        return r;
    }

    RESULT InitializeTextures(LPDIRECT3DDEVICE9 pd3device9)
    {
        RESULT r = R_OK;
        HRESULT hr = S_OK;

        CNRM(pd3device9, "DX9Material: DX9Device is Null");

        m_pppTextures = new IDirect3DTexture9**[TEXTURE_COUNT];

        // For each type of texture we now set up the textures
        for(int i = 0; i < TEXTURE_COUNT; i++)
        {
            m_pppTextures[i] = new IDirect3DTexture9*[GetTextureCount((TEXTURE_TYPE)i)];
            for(int j = 0; j < GetTextureCount((TEXTURE_TYPE)i); j++)
            {
                char *TempPath = NULL;
                r = GetTexture((TEXTURE_TYPE)i).GetTexture(j, TempPath);
                
                BSTR TexturePath;
                int lenA = lstrlenA(TempPath);
                int lenW;

                lenW = MultiByteToWideChar(CP_ACP, NULL, TempPath, -1, NULL, NULL);

                if(lenW == 0) 
                    printf("Failed to allocate BSTR!\n");

                TexturePath = SysAllocStringLen(0, lenW);
                MultiByteToWideChar(CP_ACP, NULL, TempPath, -1, TexturePath, lenW); 

                hr = D3DXCreateTextureFromFile(pd3device9, TexturePath, &m_pppTextures[i][j]);

                if(m_pppTextures[i][j] == NULL || FAILED(hr))
                {
                    printf("Failed to create texture %d\n", j);
                }   
            }
        }

        // Initialize the projection textures

Error:
        return r;
    }

    // For now we will only support single textures for each texture type. 
    // Texture 0 - Diffuse
    // Texture 1 - Bump
    RESULT SetTextures(LPDIRECT3DDEVICE9 pDevice, TEXTURE_TYPE tt)
    {
        RESULT r = R_OK;
        
        switch(tt)
        {
            case TEXTURE_DIFFUSE:
            {
                if(GetTextureCount(tt) > 0)
                {
                    pDevice->SetTexture(0, m_pppTextures[tt][0]);
                    if(GetTextureCount(tt) > 1)
                        printf("Diffuse Texture Count is %d, more than one texture is currently not supported", GetTextureCount(tt));
                }    
                else if(GetTextureCount(tt) == 0)
                    pDevice->SetTexture(0, NULL);
                else
                    CBRM((0), "Cannot have a negative diffuse texture count!");
            } break;

            case TEXTURE_BUMP:
            {
                if(GetTextureCount(tt) > 0)
                {
                    pDevice->SetTexture(1, m_pppTextures[tt][0]);
                    if(GetTextureCount(tt) > 1)
                        printf("Bump Texture Count is %d, more than one texture is currently not supported", GetTextureCount(tt));
                }    
                else if(GetTextureCount(tt) == 0)
                    pDevice->SetTexture(1, NULL);
                else
                    CBRM((0), "Cannot have a negative diffuse texture count!");
            } break;
        }

        /*
        for(int i = 0; i < GetTextureCount(tt); i++)
        {
            if(m_pppTextures[tt][i] != NULL)
            {
                pDevice->SetTexture(i, m_pppTextures[tt][i]);
            }
            else
            {
                pDevice->SetTexture(i, NULL);
            }
        }*/

Error:
        return r;
    }

private:
    // May need to change this from a dynamically allocated array to a dynamic list in the future
    // if we would like to support dynamic texture mapping at run time
    IDirect3DTexture9 ***m_pppTextures;  

public:
    /*
    // These surfaces and textures are used to store the projection maps for the material
    DX9Camera *m_pReflectionCamera;
    bool m_Reflection_f;

    DX9Camera *m_pRefractionCamera;               // not sure if this is needed since this is usually the view point with a different clipping plane
    bool m_Refraction_f;*/

};

#endif // #ifdef __WIN32__
