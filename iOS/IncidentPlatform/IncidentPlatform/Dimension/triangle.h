#pragma once

#include "DimensionPrimitives.h"

#define TRIANGLE_VERTEX_COUNT 3

typedef enum triStyle{
    TRIANGLE_UNIT_EQUILATERAL,
    TRIANGLE_EMPTY
} TRIANGLE_STYLE;

class triangle :
    public DimObject
{
public:
    
    triangle() :      
      DimObject(DIM_TRIANGLE)
    {
        memset(m_tuple, 0, sizeof(m_tuple));
        for(int i = 0; i < 3; i++) {
            m_indicies[i] = i;
            m_tuple[i].SetColor(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

      // This creation mode is for testing purposes since
      // triangles will contain all of their information in
      // the appropriate vertex
    triangle(TRIANGLE_STYLE iTriStyle) :
        triangle()
    {
        switch(iTriStyle) {
            case TRIANGLE_UNIT_EQUILATERAL: {
                m_tuple[0].set( 0.5f,  -0.5f, 0.0f);
                m_tuple[0].SetNormal(0.0f, 0.0f, 1.0f);

                m_tuple[1].set( 0.0f, 0.5f, 0.0f);
                m_tuple[1].SetNormal(0.0f, 0.0f, 1.0f);

                m_tuple[2].set(-0.5f, -0.5f, 0.0f);
                m_tuple[2].SetNormal(0.0f, 0.0f, 1.0f);
                
                break;
            }

            case TRIANGLE_EMPTY:
            default: {
                memset(m_tuple, 0, sizeof(m_tuple));
            } break;
        }
    }

    triangle(vertex *tuple) :
        triangle()
    {
        memcpy(m_tuple, tuple, sizeof(vertex) * TRIANGLE_VERTEX_COUNT);
    }

    void set(vertex *tuple)
    {
        memcpy(m_tuple, tuple, sizeof(m_tuple));
    }

    vertex *GetTuple(){ return m_tuple; }

    const char *ToString()  {
        return "Triangle";
    }

    vertex* GetVertexBuffer() {
        return m_tuple;
    }
    
    int GetVertexCount() {
        return TRIANGLE_VERTEX_COUNT;
    }

public:
    vertex m_tuple[TRIANGLE_VERTEX_COUNT];
    unsigned short m_indicies[3];
};