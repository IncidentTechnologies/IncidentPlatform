#pragma once

// A blit is the most basic building block of 2d graphics
// usually this is texure mapped with an alpha mask
// to produce complex geometry but the underlying structure 
// is a blit (a texture mapped quad)

#include "FlatObject.h"
#include "vertex.h"

#define BLIT_VERTEX_COUNT 4

typedef enum BlitType {
    BLIT_CENTERED,      // A blit created about a center origin
    BLIT_SCREEN,        // Top left is the origin, y increases in negative
    BLIT_CARTESIAN,     // Bottom left is the origin, y increase in positive
    BLIT_INVALID        // Invalid placeholder
} BLIT_TYPE;

class blit : public FlatObject {
public:
    blit(); 
    blit(point ptOrigin, float height, float width, BLIT_TYPE type, int ScreenWidth = 0, int ScreenHeight = 0); 
    blit(const blit &b); 
    blit(const point &ptA, const point &ptB); 
    ~blit();

    blit GetBlit();
    RESULT CreateBlitVertices();
    RESULT ConvertToScreen();

    float GetWidth();    
    float GetHeight();

    RESULT SetWidth(float width);
    RESULT SetHeight(float height);
    RESULT SetWidthHeight(float width, float height);

    BLIT_TYPE GetBlitType();

    const char *ToString();

    RESULT InitializeFlatObject(void *pContext);

    RESULT Render(DimensionImp *pDimImp);

    RESULT Initialize(DimensionImp *pDimImp);

    vertex* GetVertexBuffer();
    int GetVertexCount();

private:
    float m_Height;
    float m_Width;

    BLIT_TYPE m_type;

protected:
    vertex m_Vertices[BLIT_VERTEX_COUNT];
};
