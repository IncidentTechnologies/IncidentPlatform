#pragma once

// Texture is a class that encapsulates the different texturing and
// multitexturing requirements of a dimension object.  While this 
// object does not explicitly apply to all objects this object
// can be created by an object to employ the texturing capabilities.
// This object is owned with the DimObject parent class and in this 
// way any object that inherits the DimObject class will have the 
// ability to take advantage of these capabilities.

// The idea is not to enforce an interface since some objects may want to
// employ textures only in specific instances.

#define TEXTURES_MAX 16
#define TEXTURES_MAX_PATH 256

#include "RESULT.h"
#include "EHM.H"
#include "color.h"

using namespace dss;

class texture {
public:
    texture() :
      m_Textures_n(0)
    {
        memset(m_TextureFiles, 0, sizeof(char) * TEXTURES_MAX * TEXTURES_MAX_PATH);      
    }

    ~texture()
    {
        //memset(m_TextureFiles, 0, sizeof(char) * TEXTURES_MAX * TEXTURES_MAX_PATH); 
    }

    RESULT AddTexture(char *pszFilename)
    {
        RESULT r = R_SUCCESS;

        // First lets check that this texture actually exists
        FILE *TextureFile = fopen(pszFilename, "r");
        CNRM(TextureFile, "Texture file being added does not exist!");

        if(m_Textures_n < TEXTURES_MAX)
        {
            strcpy(m_TextureFiles[m_Textures_n], pszFilename);
            m_Textures_n++;
        }
        else
        {
            printf("Too many textures could not add:%s\n", pszFilename);
            return R_TEXTURE_TOO_MANY;
        }

Error:
        if(TextureFile != NULL) fclose(TextureFile);            
        return r;
    }

    RESULT GetTexture(int id, char* &pszTextureFile)
    {
        RESULT r = R_SUCCESS;
        pszTextureFile = NULL;

        CBRM((id < TEXTURES_MAX), "ID TOO BIG MAX");
        CBRM((id < m_Textures_n), "Not that many IDs");

        pszTextureFile = m_TextureFiles[id];

Error:
        return r;
    }
    
    int GetTextureCount(){ return m_Textures_n; }

    RESULT SetColor(color c){ m_color = c; return R_SUCCESS; }
    color GetColor(){ return m_color; }

private:
    char m_TextureFiles[TEXTURES_MAX][TEXTURES_MAX_PATH];
    int m_Textures_n;
    color m_color;
};