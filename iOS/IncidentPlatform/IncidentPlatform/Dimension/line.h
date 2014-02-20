#pragma once

// Line is a special object since it can either be flat or dimensioned.  
// For now we'll implement it as a flat object and then modify FlatObject
// to act as an adapter that overrides specific DimObject functions.  This
// should still work since the child precedes the parent in inheritance 

#include "FlatObject.h"
#include <math.h>
#include "blit.h"

// Line is actually represented visually as a quad
// since this is faster on most hardware than a line strip
#define LINE_VERTEX_COUNT 4

class line :
    public FlatObject
{
public:
    line(point ptOrigin = point(), point ptEnd = point(), float LineWidth = 0.0f, int ScreenWidth = 0, int ScreenHeight = 0) :
      FlatObject(ptOrigin, FLAT_LINE, ScreenWidth, ScreenHeight),
      m_ptEnd(ptEnd),
      m_LineWidth(LineWidth)
    {
        CreateLineVertices();
    }

    line(const line &l) :
      FlatObject(l),
      m_ptEnd(l.m_ptEnd),
      m_LineWidth(l.m_LineWidth)
    {
        CreateLineVertices();
    }

    ~line()
    {
        /*empty stub*/
    }

    RESULT ConvertToScreen()
    {
        RESULT r = R_SUCCESS;

        CBRM(m_Screen_f, "line: ConvertToScreen called before screen specs set");
        m_ptOrigin = m_ptOrigin.ToScreenCoordinates(m_ScreenWidth, m_ScreenHeight);
        m_ptEnd = m_ptEnd.ToScreenCoordinates(m_ScreenWidth, m_ScreenHeight);

Error:
        return r;
    }

    RESULT CreateLineVertices()
    {
        RESULT r = R_SUCCESS;

        float dx = pX(m_ptEnd) - pX(m_ptOrigin);
        float dy = pY(m_ptEnd) - pY(m_ptOrigin);
        float l = sqrt(dx*dx + dy*dy);
        float dx_n = dx / l;
        float dy_n = dy / l;
        float m = dy / dx;
                
        float lwx = m_LineWidth / 2;
        float lwy = m_LineWidth / 2;      

        if(m_Screen_f && m_ScreenHeight != 0 && m_ScreenWidth != 0)
        {
            lwx = lwx / m_ScreenWidth;
            lwy = lwy / m_ScreenHeight;
            if(lwy < 0) lwy *= -1;
        }

        float dxTop = dy_n * lwx;
        float dyTop = -dx_n * lwy;
        float dxBottom = -dy_n * lwx;
        float dyBottom = dx_n * lwy;



        // Top Left Vertex is origin
        m_Vertices[0].set(dxTop, dyTop, 0.0f, 1.0f);
        m_Vertices[0].SetNormal(0.0f, 0.0f, 1.0f);
        m_Vertices[0].SetUV(0.0f, 0.0f);

        // Top Right Vertex
        m_Vertices[1].set(dx + dxTop, dy + dyTop, 0.0f, 1.0f);
        m_Vertices[1].SetNormal(0.0f, 0.0f, 1.0f);
        m_Vertices[1].SetUV(1.0f, 0.0f);

        // Bottom Left Vertex
        m_Vertices[2].set(dxBottom, dyBottom, 0.0f, 1.0f);
        m_Vertices[2].SetNormal(0.0f, 0.0f, 1.0f);
        m_Vertices[2].SetUV(0.0f, 1.0f);

        // Bottom Right Vertex
        m_Vertices[3].set(dx + dxBottom, dy + dyBottom, 0.0f, 1.0f);
        m_Vertices[3].SetNormal(0.0f, 0.0f, 1.0f);
        m_Vertices[3].SetUV(1.0f, 1.0f);

        // Reference Line Verts
        // Bottom Left Vertex
        m_LineVertices[0].set(0.0f, 0.0f, 0.0f, 1.0f);
        m_LineVertices[0].SetNormal(0.0f, 0.0f, 1.0f);
        m_LineVertices[0].SetUV(0.0f, 0.0f);

        // Bottom Right Vertex
        m_LineVertices[1].set(dx, dy, 0.0f, 1.0f);
        m_LineVertices[1].SetNormal(0.0f, 0.0f, 1.0f);
        m_LineVertices[1].SetUV(1.0f, 1.0f);

Error:
        return r;
    }

    float GetLineWidth(){ return m_LineWidth; }
    RESULT SetLineWidth(float LineWidth){ m_LineWidth = LineWidth; return R_SUCCESS; }
    float GetLineLength()
    {
        float x = pX(m_ptEnd) - pX(m_ptOrigin);
        float y = pY(m_ptEnd) - pY(m_ptOrigin);
        float z = pZ(m_ptEnd) - pZ(m_ptOrigin);

        return sqrt(x*x + y*y + z*z);
    }

    const char *ToString()
    {
        return "line";
    }

    blit GetBlit()
    {
        return blit(m_ptOrigin, m_ptEnd);
    }

    RESULT SetEndPoint(point ptEnd)
    { 
        RESULT r = R_SUCCESS;

        if(m_ptEnd != ptEnd) 
        {
            m_ptEnd = ptEnd.ToScreenCoordinates(m_ScreenWidth, m_ScreenHeight);           
            CR(CreateLineVertices());
        }

Error:
        return r; 
    }

    RESULT InitializeFlatObject(void *pContext)
    {
        printf("Initialize Line Object\n");
        return R_NOT_IMPLEMENTED;
    }

    RESULT Render(DimensionImp *pDimImp)
    {
        return R_NOT_IMPLEMENTED;
    }

    RESULT Initialize(DimensionImp *pDimImp)
    {
        return R_NOT_IMPLEMENTED;
    }

    vertex* GetVertexBuffer()
    {
        return m_Vertices;
    }
    int GetVertexCount()
    {
        return LINE_VERTEX_COUNT;
    }

private:
    point m_ptEnd;
    float m_LineWidth; 

protected:
    vertex m_Vertices[LINE_VERTEX_COUNT];
    vertex m_LineVertices[2];                   // Reference line verts
};