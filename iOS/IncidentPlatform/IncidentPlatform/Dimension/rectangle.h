#pragma once

// The rectangle is, as the name suggests, a 2d rectangle representation
// as with the other 2d objects the positioning and sizing of the rectangle
// will be relative to the screen co-ordinates and such.  These are meant
// for in game graphics and not for the actual 3d graphics system.  

// NOTE: A rectangle is not an arbitrary quadrilateral, rhombic shapes, parallelograms
// etc must be created brute force with triangles or with planned future support.  Also
// a square is a rectangle but a rectangle must not be a square.

// DEPRECATE: BLIT //
// DEPRECATE: BLIT //
// DEPRECATE: BLIT //

#define RECTANGLE_VERTEX_COUNT 4

class rectangle :
    public DimObject
{
public:
    rectangle() :
      DimObject(DIM_RECTANGLE),
      m_Height(0),
      m_Width(0)
    {
        memset(m_Vertices, 0, sizeof(vertex) * RECTANGLE_VERTEX_COUNT);
    }

    rectangle(point origin, float height, float width) :
      DimObject(origin, DIM_RECTANGLE),
      m_Height(height),
      m_Width(width)
    {
        // Set up the rectangle verts here
        CreateRectangleVertices();
    }

    ~rectangle()
    {
        /* empty stub */
    }

    RESULT CreateRectangleVertices()
    {
        RESULT r = R_SUCCESS;

        // Horizontal Rect
        // Top Left Vertex is origin
        m_Vertices[0].set(-m_Width/2, 0.0f, m_Height/2, 1.0f);
        m_Vertices[0].SetNormal(0.0f, 1.0f, 0.0f);
        m_Vertices[0].SetUV(0.0f, 0.0f);

        // Top Right Vertex
        m_Vertices[1].set(m_Width/2, 0.0f, m_Height/2, 1.0f);
        m_Vertices[1].SetNormal(0.0f, 1.0f, 0.0f);
        m_Vertices[1].SetUV(1.0f, 0.0f);

        // Bottom Left Vertex
        m_Vertices[2].set(-m_Width/2, 0.0f, -m_Height/2, 1.0f);
        m_Vertices[2].SetNormal(0.0f, 1.0f, 0.0f);
        m_Vertices[2].SetUV(0.0f, 1.0f);

        // Bottom Right Vertex
        m_Vertices[3].set(m_Width/2, 0.0f, -m_Height/2, 1.0f);
        m_Vertices[3].SetNormal(0.0f, 1.0f, 0.0f);
        m_Vertices[3].SetUV(1.0f, 1.0f);

        for(int i = 0; i < RECTANGLE_VERTEX_COUNT; i++)
        {
            m_Vertices[i].SetColor((unsigned char)0xFF, (unsigned char)0xFF, (unsigned char)0xFF, (unsigned char)0xFF);
        }

        /* vertical rect
        // Top Left Vertex is origin
        m_Vertices[1].set(-m_Width/2, m_Height/2, 0.0f, 1.0f);
        m_Vertices[1].SetNormal(0.0f, 0.0f, 1.0f);
        m_Vertices[1].SetUV(0.0f, 0.0f);

        // Top Right Vertex
        m_Vertices[0].set(m_Width/2, m_Height/2, 0.0f, 1.0f);
        m_Vertices[0].SetNormal(0.0f, 0.0f, 1.0f);
        m_Vertices[0].SetUV(1.0f, 0.0f);

        // Bottom Left Vertex
        m_Vertices[3].set(-m_Width/2, -m_Height/2, 0.0f, 1.0f);
        m_Vertices[3].SetNormal(0.0f, 0.0f, 1.0f);
        m_Vertices[3].SetUV(0.0f, 1.0f);

        // Bottom Right Vertex
        m_Vertices[2].set(m_Width/2, -m_Height/2, 0.0f, 1.0f);
        m_Vertices[2].SetNormal(0.0f, 0.0f, 1.0f);
        m_Vertices[2].SetUV(1.0f, 1.0f);
        */

Error:
        return r;
    }

    point GetOrigin(){ return m_ptOrigin; }
    float GetHeight(){ return m_Height; }
    float GetWidth(){ return m_Width; }

    const char *ToString() 
    {
        return "Rectangle";
    }

    vertex* GetVertexBuffer()
    {
        return m_Vertices;
    }
    int GetVertexCount()
    {
        return RECTANGLE_VERTEX_COUNT;
    }

private:
    float m_Height;
    float m_Width;

protected:
    vertex m_Vertices[RECTANGLE_VERTEX_COUNT];
};