// Defines a 3d volume 
//           
//        _______________
//       /              /|
//      /              / | y - height
//     /______________/  |
//     |             |  /
//     |             | / z - width
//     |_____________|/
//           x - length
//
// Where x is tuple[0], y is tuple[1] is z it tuple[2] in
// the dimensional vector.  The vector is simply used as a 
// convinient way to store the information about the bounded
// space

#pragma once

#include "DimObject.h"
#include "nuke.h"

#define VOLUME_VERTEX_COUNT 24

class volume :
    public DimObject
{
public:
    volume() :
      DimObject(DIM_VOLUME),
      m_fLength(1.0f),
      m_fHeight(1.0f),
      m_fWidth(1.0f)
    {
        //m_ptOrigin.set(0.0f, 0.0f, 0.0f);
        SetVolume();
    }

    volume(float fLength, float fHeight, float fWidth) :
      DimObject(DIM_VOLUME),
      m_fLength(fLength),
      m_fHeight(fHeight),
      m_fWidth(fWidth)
    {
        m_ptOrigin.set(0.0f, 0.0f, 0.0f);
        SetVolume();
    }

    volume(point ptOrigin) :
      DimObject(ptOrigin, DIM_VOLUME),
      m_fLength(1.0f),
      m_fHeight(1.0f),
      m_fWidth(1.0f)
    {
        SetVolume();
    }

    volume(point ptOrigin, float fLength, float fHeight, float fWidth) :
      DimObject(ptOrigin, DIM_VOLUME),
      m_fLength(fLength),
      m_fHeight(fHeight),
      m_fWidth(fWidth)
    {
        SetVolume();
    }
        

    void SetVolume()
    {
        // Some temporary varibles to make things easier to code
        float x = m_fLength/2;
        float y = m_fHeight/2;
        float z = m_fWidth/2;

        point ptOrigin(0, 0, 0);
        
        // Back Face
        // Normal -k
        m_vertices[0].set(ptOrigin + point(-x, y, -z));
        m_vertices[0].SetNormal(0.0f, 0.0f, -1.0f);
        m_vertices[1].set(ptOrigin + point(x, y, -z));
        m_vertices[1].SetNormal(0.0f, 0.0f, -1.0f);
        m_vertices[2].set(ptOrigin + point(-x,-y, -z));
        m_vertices[2].SetNormal(0.0f, 0.0f, -1.0f);
        m_vertices[3].set(ptOrigin + point(x,-y, -z));
        m_vertices[3].SetNormal(0.0f, 0.0f, -1.0f);

        // Front Face
        // Normal is k
        m_vertices[4].set(ptOrigin + point(-x, y, z));
        m_vertices[4].SetNormal(0.0f, 0.0f, 1.0f);
        m_vertices[5].set(ptOrigin + point(-x,-y, z));
        m_vertices[5].SetNormal(0.0f, 0.0f, 1.0f);
        m_vertices[6].set(ptOrigin + point(x, y, z));
        m_vertices[6].SetNormal(0.0f, 0.0f, 1.0f);
        m_vertices[7].set(ptOrigin + point( x,-y, z));
        m_vertices[7].SetNormal(0.0f, 0.0f, 1.0f);

        // Top Face
        // Normal is j
        m_vertices[8].set(ptOrigin + point(-x, y,  z));
        m_vertices[8].SetNormal(0.0f, 1.0f, 0.0f);
        m_vertices[9].set(ptOrigin + point( x, y,  z));
        m_vertices[9].SetNormal(0.0f, 1.0f, 0.0f);
        m_vertices[10].set(ptOrigin + point(-x, y, -z));
        m_vertices[10].SetNormal(0.0f, 1.0f, 0.0f);
        m_vertices[11].set(ptOrigin + point(x, y, -z));
        m_vertices[11].SetNormal(0.0f, 1.0f, 0.0f);

        // Bottom Face
        // Normal is -j
        m_vertices[12].set(ptOrigin + point(-x,-y,  z));
        m_vertices[12].SetNormal(0.0f, -1.0f, 0.0f);
        m_vertices[13].set(ptOrigin + point(-x,-y, -z));
        m_vertices[13].SetNormal(0.0f, -1.0f, 0.0f);
        m_vertices[14].set(ptOrigin + point(x,-y,  z));
        m_vertices[14].SetNormal(0.0f, -1.0f, 0.0f);
        m_vertices[15].set(ptOrigin + point( x,-y, -z));
        m_vertices[15].SetNormal(0.0f, -1.0f, 0.0f);

        // Positive X Face
        // Normal is i
        m_vertices[16].set(ptOrigin + point(x, y, -z));
        m_vertices[16].SetNormal(1.0f, 0.0f, 0.0f);
        m_vertices[17].set(ptOrigin + point( x, y,  z));
        m_vertices[17].SetNormal(1.0f, 0.0f, 0.0f);
        m_vertices[18].set(ptOrigin + point(x,-y, -z));
        m_vertices[18].SetNormal(1.0f, 0.0f, 0.0f);
        m_vertices[19].set(ptOrigin + point( x,-y,  z));
        m_vertices[19].SetNormal(1.0f, 0.0f, 0.0f);

        // Negative X Face
        // Normal is -i
        m_vertices[20].set(ptOrigin + point(-x, y, -z));
        m_vertices[20].SetNormal(-1.0f, 0.0f, 0.0f);
        m_vertices[21].set(ptOrigin + point(-x,-y, -z));
        m_vertices[21].SetNormal(-1.0f, 0.0f, 0.0f);
        m_vertices[22].set(ptOrigin + point(-x, y,  z));
        m_vertices[22].SetNormal(-1.0f, 0.0f, 0.0f);
        m_vertices[23].set(ptOrigin + point(-x,-y,  z));
        m_vertices[23].SetNormal(-1.0f, 0.0f, 0.0f);
    }

    // Collision Detection
    // Used by Nuclea
    // Nuke is essentially a function that will return whether or not 
    // two objects are overlapping.  It can be provided with a vector
    // to register between timestep collisons
    bool Nuke(point ptPoint)
    {
        // First we get the point in reference of the volume
        vector ptRef = ptPoint - m_ptOrigin;

        // Now check if point is in within bounds
        if( (ptRef.tuple[DIM_X] <= m_fLength/2 && ptRef.tuple[DIM_X] >= -m_fLength/2) &&
            (ptRef.tuple[DIM_Y] <= m_fHeight/2 && ptRef.tuple[DIM_Y] >= -m_fHeight/2) &&
            (ptRef.tuple[DIM_Z] <= m_fWidth/2 && ptRef.tuple[DIM_Z] >= -m_fWidth/2) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Nuke(sphere Sphere)
    {
        // Implement Arvo's algorithm
        // Reset Reference
        vector ptRef = Sphere.GetOrigin() - m_ptOrigin;
        float dmin = 0;
        float min[3] = {-m_fLength/2, -m_fHeight/2, -m_fWidth/2};
        float max[3] = {m_fLength/2, m_fHeight/2, m_fWidth/2};

        for(int i = 0; i < 3; i++)
        {
            if(ptRef.tuple[i] < min[i])
            {
                dmin += pow(ptRef.tuple[i] - min[i], 2);
            }
            else if(ptRef.tuple[i] > max[i])
            {
                dmin += pow(ptRef.tuple[i] - max[i], 2);      
            }   
        }

        if(dmin <= pow(Sphere.GetRadius(), 2))
        {
            for(int i=0;i<VOLUME_VERTEX_COUNT;i++)
            {
                m_vertices[i].SetColor(1.0f,1.0f,1.0f,1.0f);
            }
            return true;
        }
        else
        {
            for(int i=0;i<VOLUME_VERTEX_COUNT;i++)
            {
                m_vertices[i].SetColor(1.0f,1.0f,1.0f,1.0f);
            }
            return false;
        }
    }

    float GetLength() { return m_fLength; }
    float GetHeight() { return m_fHeight; }
    float GetWidth() { return m_fWidth; }

    const char *ToString() 
    {
        return "Volume";
    }

    ~volume()
    {
        
    }

    vertex* GetVertexBuffer()
    {
        return m_vertices;
    }
    int GetVertexCount()
    {
        return VOLUME_VERTEX_COUNT;
    }

protected:
    float m_fLength;
    float m_fHeight;
    float m_fWidth;
    
    // There will be a total of 8 vertices
    vertex m_vertices[VOLUME_VERTEX_COUNT];
};

