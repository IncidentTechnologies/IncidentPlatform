#pragma once

// HTerrain is a height mapped based terrain class which will load it's 
// data out of a image file

#define HTERRAIN_DEFAULT_LONG 128
#define HTERRAIN_DEFAULT_LAT 128

#include "ImageBMP.h"

typedef enum HTerrainType
{
    HTERRAIN_HEIGHTMAP,
    HTERRAIN_PROCEDURAL,
    HTERRIAN_INVALID
} HTERRAIN_TYPE;

typedef enum HTerrainProceduralType
{
    HTERRAIN_PROCEDURAL_INVALID
} HTERRAIN_PROCEDURAL_TYPE;

class hterrain :
    public DimObject
{
public:
    hterrain() :
      DimObject(DIM_HTERRAIN),
      m_LongSections(HTERRAIN_DEFAULT_LONG),
      m_LatSections(HTERRAIN_DEFAULT_LAT),
      m_Width(10.0f),
      m_Height(1.0f),
      m_Length(10.0f),
      m_TextureRepetitions(1.0f)
    {
        RESULT r = Generate(0.0f);
    }

    hterrain(point ptOrigin, float Width, float Length, float Height) :
      DimObject(ptOrigin, DIM_HTERRAIN),
      m_LongSections(HTERRAIN_DEFAULT_LONG),
      m_LatSections(HTERRAIN_DEFAULT_LAT),
      m_Width(Width),
      m_Length(Length),
      m_Height(Height),
      m_TextureRepetitions(10.0f)
    {
        RESULT r = Generate(0.0f);
    }

    ~hterrain()
    {
        /* emptystub */
    }   

    // Range of x and z are from 0.0f to 1.0f
    RESULT GenerateSinXSinZVertex(vertex &r_v, float x, float z, float x_reps, float z_reps, float width, float length, float height, float offset)
    {
        RESULT r = R_SUCCESS;

        float y = sin(x * PI + offset) + sin(z * PI + offset);
          
        r_v.set(x * width - (width / 2), y * height, z * length - (length / 2));

        float dXVal = -cos(x * PI + offset);
        float dYVal = 1;
        float dZVal = -cos(z * PI + offset);

        r_v.SetNormal(dXVal, dYVal, dZVal);
        r_v.Normalize();

        float U = x;
        float V = z;

        r_v.SetUV(U, V);
        r_v.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

Error:
        return r;
    }

    RESULT Generate(float offset)
    {
        RESULT r = R_SUCCESS;
    
        m_Vertices = new vertex[m_LongSections * m_LatSections];
        memset(m_Vertices, 0, sizeof(vertex) * m_LongSections * m_LatSections);

        ImageBMP *HeightMap = new ImageBMP("./data/heightmap.bmp");
        //ImageBMP *HeightMap = new ImageBMP("./data/hmap3.bmp");


        for(int i = 0; i < m_LatSections; i++)
        {
            for(int j = 0; j < m_LongSections; j++)
            {
                int index = (i * m_LongSections) + j;

                //GenerateSinXSinZVertex(m_Vertices[index], (float)(j / m_LatSections), float(i / m_LongSections), 1.0f, 1.0f, m_Width, m_Length, m_Height, 0.0f);
                float XVal = (float)(j)/(float)(m_LatSections);
                float ZVal = (float)(i)/(float)(m_LongSections);
                float YVal = (float)(HeightMap->GetValue(XVal, ZVal) / 255.0f);

                m_Vertices[index].set(XVal * m_Width - (m_Width / 2), YVal * m_Height, ZVal * m_Length - (m_Length / 2));
                m_Vertices[index].SetNormal(0, 1, 0);              

                int ui = (int)(i * m_TextureRepetitions) % m_LongSections;
                int vi = (int)(j * m_TextureRepetitions) % m_LatSections;

                float U = (float)(ui) / float(m_LongSections);
                float V = (float)(vi) / float(m_LatSections);

                m_Vertices[index].SetUV(U, V);
                
                /*

                float XVal = (float)(j) / float(m_LatSections);// / m_LatSections);
                float ZVal = (float)(i) / float(m_LongSections);// / m_LongSections);     
                float YVal = sin(XVal * PI + offset) + sin(ZVal * PI + offset);
                          
                m_Vertices[index].set(XVal * m_Width - (m_Width / 2), YVal * m_Height, ZVal * m_Length - (m_Length / 2));

                float dXVal = -cos(XVal * PI + offset);
                float dYVal = 1;//cos(XVal + offset) + cos(ZVal + offset);
                float dZVal = -cos(ZVal * PI + offset);

                m_Vertices[index].SetNormal(dXVal, dYVal, dZVal);
                //m_Vertices[index].SetNormal(0, -1, 0);

                m_Vertices[index].Normalize();

                float U = (float)(i / m_LongSections);
                float V = (float)(j / m_LatSections);

                m_Vertices[index].SetUV(U, V);

                */

                m_Vertices[index].SetColor(1.0f, 1.0f, 1.0f, 1.0f);
            }
        }

        for(int i = 0; i < m_LatSections; i++)
        {
            for(int j = 0; j < m_LongSections; j++)
            {
                int index = (i * m_LongSections) + j;
                vector A, B, C, D;
                
                if(j != m_LongSections - 1)
                    A = m_Vertices[index + 1] - m_Vertices[index];

                if(i != m_LatSections - 1)
                    B = m_Vertices[((i + 1) * m_LongSections) + j] - m_Vertices[index];

                if(j != 0)
                    C = m_Vertices[index - 1] - m_Vertices[index];

                if(i != 0)
                    D = m_Vertices[((i - 1) * m_LongSections) + j] - m_Vertices[index];

                vector N = (B % A) + (C % B) + (D % C) + (A % D);
                N.normalize();

                m_Vertices[index].SetNormal(N);
            }
        }

Error:
        return r;
    }

    int GetLongSections(){ return m_LongSections; }
    int GetLatSections(){ return m_LatSections; }
    float GetWidth(){ return m_Width; }
    float GetLength(){ return m_Length; }
    float GetHeight(){ return m_Height; }

    const char *ToString() 
    {
        return "Height Mapped Terrain";
    }

    vertex* GetVertexBuffer()
    {
        return m_Vertices;
    }
    int GetVertexCount()
    {
        return (m_LongSections * m_LatSections);
    }

private:
    char *m_pszHeightFilename;    // contains the height map file name

protected:
    int m_LongSections;
    int m_LatSections;

    float m_Width;
    float m_Length;
    float m_Height;

    float m_TextureRepetitions;

protected:
    vertex *m_Vertices;
};