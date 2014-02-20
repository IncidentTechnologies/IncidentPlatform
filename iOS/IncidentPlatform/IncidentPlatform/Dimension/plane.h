#pragma once

// Plane is a simple grid based plane.  This differs from the 
// rectangle class where rectangle is a single pair of triangles 
// the plane can be many

#define PLANE_DEFAULT_LONG 16
#define PLANE_DEFAULT_LAT 16

#define PLANE_DEFAULT_WIDTH 10.0f
#define PLANE_DEFAULT_LENGTH 10.0f
#define PLANE_DEFAULT_TEXTURE_REPS 1.0f

#include "vertex.h"
#include "DimObject.h"

class plane :
    public DimObject
{
public:
    plane(bool GenerateVertices = true, bool GenerateMaterial = true) :
        DimObject(DIM_PLANE, NULL, GenerateMaterial),
        m_LongSections(PLANE_DEFAULT_LONG),
        m_LatSections(PLANE_DEFAULT_LAT),
        m_Width(PLANE_DEFAULT_WIDTH),
        m_Length(PLANE_DEFAULT_LENGTH),
        m_TextureRepetitions(PLANE_DEFAULT_TEXTURE_REPS),
        m_Vertices(NULL)
        //m_Normal(vector(0.0f, 1.0f, 0.0f))
    {
        RESULT r = R_SUCCESS;
        
        if(GenerateVertices)
        {
            r = Generate();
            m_Normal.set(0.0f, 1.0f, 0.0f);
        }
    }

    plane(point ptOrigin, bool GenerateVertices = true, bool GenerateMaterial = true) :
        DimObject(ptOrigin, DIM_PLANE, NULL, GenerateMaterial),
        m_LongSections(PLANE_DEFAULT_LONG),
        m_LatSections(PLANE_DEFAULT_LAT),
        m_Width(PLANE_DEFAULT_WIDTH),
        m_Length(PLANE_DEFAULT_LENGTH),
        m_TextureRepetitions(PLANE_DEFAULT_TEXTURE_REPS),
        m_Vertices(NULL)
        //m_Normal(vector(0.0f, 1.0f, 0.0f))
    {
        RESULT r = R_SUCCESS;
        
        if(GenerateVertices)
            r = Generate();
        
        m_Normal.set(0.0f, 1.0f, 0.0f);
    }

    plane(point ptOrigin, vector Normal, bool GenerateVertices = true, bool GenerateMaterial = true) :
        DimObject(ptOrigin, DIM_PLANE, NULL, GenerateMaterial),
        m_LongSections(PLANE_DEFAULT_LONG),
        m_LatSections(PLANE_DEFAULT_LAT),
        m_Width(PLANE_DEFAULT_WIDTH),
        m_Length(PLANE_DEFAULT_LENGTH),
        m_TextureRepetitions(PLANE_DEFAULT_TEXTURE_REPS),
        m_Vertices(NULL),
        m_Normal(Normal)
    {
        RESULT r = R_SUCCESS;
        
        if(GenerateVertices)        
            r = Generate();
        
    }

    plane(point ptOrigin, float Width, float Length, bool GenerateVertices = true, bool GenerateMaterial = true) :
      DimObject(ptOrigin, DIM_PLANE, NULL, GenerateMaterial),
      m_LongSections(PLANE_DEFAULT_LONG),
      m_LatSections(PLANE_DEFAULT_LAT),
      m_Width(Width),
      m_Length(Length),
      m_TextureRepetitions(PLANE_DEFAULT_TEXTURE_REPS),
      m_Vertices(NULL)
      //m_Normal(vector(0.0f, 1.0f, 0.0f))
    {
        RESULT r = R_SUCCESS;

        if(GenerateVertices)
            r = Generate();

        m_Normal.set(0.0f, 1.0f, 0.0f);
    }

    plane(point ptOrigin, float Width, float Length, int TexReps, bool GenerateVertices = true, bool GenerateMaterial = true) :
        DimObject(ptOrigin, DIM_PLANE, NULL, GenerateMaterial),
        m_LongSections(PLANE_DEFAULT_LONG),
        m_LatSections(PLANE_DEFAULT_LAT),
        m_Width(Width),
        m_Length(Length),
        m_TextureRepetitions(TexReps),
        m_Vertices(NULL)
        //m_Normal(vector(0.0f, 1.0f, 0.0f))
    {
        RESULT r = R_SUCCESS;

        if(GenerateVertices)
            r = Generate();

        m_Normal.set(0.0f, 1.0f, 0.0f);
    }

    ~plane()
    {
        if(m_Vertices != NULL)
        {        
            delete [] m_Vertices;
            m_Vertices = NULL;
        }
    }   

    plane *GetPlane()
    {
        return this;
    }

    RESULT SetPlane(point ptOrigin, vector vOrientation)
    {
        RESULT r = R_NOT_IMPLEMENTED;

        /*TODO*/
Error:
        return r;
    }

    RESULT Generate()
    {
        RESULT r = R_SUCCESS;

        m_Vertices = new vertex[m_LongSections * m_LatSections];
        memset(m_Vertices, 0, sizeof(vertex) * m_LongSections * m_LatSections);

        for(int i = 0; i < m_LatSections; i++)
        {
            for(int j = 0; j < m_LongSections; j++)
            {
                int index = (i * m_LongSections) + j;

                float XVal = (float)(j)/(float)(m_LongSections);
                float ZVal = (float)(i)/(float)(m_LatSections);
                float YVal = 0.0f;

                m_Vertices[index].set(XVal * m_Width - (m_Width / 2), YVal, ZVal * m_Length - (m_Length / 2));
                m_Vertices[index].SetNormal(0.0f, 1.0f, 0.0f);   
                m_Vertices[index].SetTangent(1.0f, 0.0f, 0.0f);

                int ui = (int)(j) % m_LongSections;
                int vi = (int)(i) % m_LatSections;

                float U = ((float)(ui) / float(m_LongSections)) * m_TextureRepetitions;
                float V = ((float)(vi) / float(m_LatSections)) * m_TextureRepetitions;

                m_Vertices[index].SetUV(U, V);
                m_Vertices[index].SetColor(1.0f, 1.0f, 1.0f, 1.0f);
            }
        }

        /*
        for(int i = 0; i < m_LatSections; i++)
        {
            for(int j = 0; j < m_LongSections; j++)
            {
                int index = (i * m_LongSections) + j;
                if(j != 0)
                {
                    float du = m_Vertices[index].m_UV.tuple[1] - m_Vertices[index - 1].m_UV.tuple[1];
                    vector dv = m_Vertices[index] - m_Vertices[index - 1];
                    dv = dv * du;
                    dv.normalize();
                    m_Vertices[index].SetTangent(dv);
                }
            }
        }
        */
Error:
        return r;
    }

    int GetLongSections(){ return m_LongSections; }
    int GetLatSections(){ return m_LatSections; }
    float GetWidth(){ return m_Width; }
    float GetLength(){ return m_Length; }

    int GetTexReps(){ return m_TextureRepetitions; }

    vector GetNormal(){ return m_Normal; }

    const char *ToString() 
    {
        return "Plane";
    }

    vertex* GetVertexBuffer()
    {
        return m_Vertices;
    }
    int GetVertexCount()
    {
        return (m_LongSections * m_LatSections);
    }

protected:
    int m_LongSections;
    int m_LatSections;

    float m_Width;
    float m_Length;

    float m_TextureRepetitions;

protected:
    vertex *m_Vertices;
public:
    vector m_Normal;        // this along with the origin will define the plane!
};