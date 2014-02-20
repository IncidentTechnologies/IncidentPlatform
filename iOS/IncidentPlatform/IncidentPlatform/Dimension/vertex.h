// vertex differs from point in a very specific way
// a vertex will derive most of it's functionality from
// point.  However, a point can contain more information
// than a point such as color, normals, and texture coordinates.

#pragma once

#include "RESULT.h"
#include "color.h"
#include "colord.h"
#include "point2.h"

using namespace dss;

class vertex : public point {
public:
    vertex() :
      point(),
      m_Color(),
      m_Normal(),
      m_UV(),
      m_Size(1.0f)
    {
        m_Color = color(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    vertex(point pt) :
        point(pt),
        m_Color(),
        m_Normal(),
        m_UV(),
        m_Size(1.0f)
    {
        m_Color = color(1.0f, 1.0f, 1.0f, 1.0f);
    }

    ~vertex(){/*empty stub*/}

    void SetNormal(float x, float y, float z){ m_Normal.set(x, y, z); }
    void SetNormal(vector v){ m_Normal = v; }
    void SetTangent(vector v){ m_Tangent = v; }
    void SetTangent(float x, float y, float z){ m_Tangent.set(x, y, z); }

    // Will normalize the normal
    RESULT Normalize(){ m_Normal.normalize(); return R_SUCCESS; }
    RESULT SetColor(float Tuple[COLOR_DIMENSIONS]){ m_Color = color(Tuple); return R_SUCCESS; }
    RESULT SetColor(unsigned char A, unsigned char R, unsigned char G, unsigned char B){ m_Color = color(A, R, G, B); return R_SUCCESS; }
    RESULT SetColor(float A, float R, float G, float B){ m_Color = color(A, R, G, B); return R_SUCCESS; }
    RESULT SetColor(color c){ m_Color = c; return R_SUCCESS; }
    RESULT SetUV(float x, float y){ m_UV = point2(x, y); return R_SUCCESS; }
    RESULT SetUV(point2 uv){ m_UV = uv; return R_SUCCESS; }
    RESULT SetSize(float size){ m_Size = size; return R_SUCCESS; }
    float GetSize(){ return m_Size; }

public:
    // Vertex inherits point should this be changed?
    vector m_Normal;    // The normal of the vertex
    vector m_Tangent;   // The Tangent of the vertex
    //colord m_Color;      // The color of the vertex
    color m_Color;
    point2 m_UV;        // The UV coordinates of the vertex
    float m_Size;       // Point size of the vertex
};