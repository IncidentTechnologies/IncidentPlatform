#include "blit.h"
#include <math.h>

blit::blit() :
  FlatObject(FLAT_BLIT),
  m_type(BLIT_CENTERED),
  m_Height(0.0f),
  m_Width(0.0f)
{
    memset(m_Vertices, 0, sizeof(vertex) * BLIT_VERTEX_COUNT);
}

blit::blit(point ptOrigin, float height, float width, BLIT_TYPE type, int ScreenWidth, int ScreenHeight) :
    FlatObject(ptOrigin, FLAT_BLIT, ScreenWidth, ScreenHeight),
    m_type(type),
    m_Height(height),
    m_Width(width)
{
    CreateBlitVertices();
}

blit::blit(const blit &b) :
    FlatObject(b),
    m_type(b.m_type),
    m_Height(b.m_Height),
    m_Width(b.m_Width)
{
    CreateBlitVertices();
}

    
blit::blit(const point &ptA, const point &ptB) :
    FlatObject(FLAT_BLIT),
    m_type(BLIT_SCREEN)
{
    m_Height = fabs(pY(ptB) - pY(ptA));
    m_Width = fabs(pX(ptB) - pY(ptA));
    m_ptOrigin = point(fmin(pX(ptB), pX(ptA)), fmin(pY(ptB), pY(ptA)));
    CreateBlitVertices();
}

blit::~blit() {
    /*empty stub*/
}

blit blit::GetBlit() {
    return blit(*this);
}

RESULT blit::CreateBlitVertices() {
    RESULT r = R_SUCCESS;

    float MinX, MinY, MaxX, MaxY;

    switch(m_type) {
        case BLIT_CENTERED: {
            MinX = -m_Width / 2;
            MaxX = m_Width / 2;
            MinY = m_Height / 2;
            MaxY = -m_Height / 2;
        } break;

        case BLIT_SCREEN: {
            MinX = 0;
            MaxX = m_Width;
            MinY = 0.0f;
            MaxY = m_Height;
        } break;

        case BLIT_CARTESIAN: {
            MinX = 0.0f;
            MaxX = m_Width;
            MinY = m_Height;
            MaxY = 0.0f;
        } break;
    }

    // Top Left Vertex is origin
    m_Vertices[0].set(MinX, MinY, 0.0f, 1.0f);
    m_Vertices[0].SetNormal(0.0f, 0.0f, 1.0f);
    m_Vertices[0].SetUV(0.0f, 0.0f);

    // Top Right Vertex
    m_Vertices[1].set(MaxX, MinY, 0.0f, 1.0f);
    m_Vertices[1].SetNormal(0.0f, 0.0f, 1.0f);
    m_Vertices[1].SetUV(1.0f, 0.0f);

    // Bottom Left Vertex
    m_Vertices[2].set(MinX, MaxY, 0.0f, 1.0f);
    m_Vertices[2].SetNormal(0.0f, 0.0f, 1.0f);
    m_Vertices[2].SetUV(0.0f, 1.0f);

    // Bottom Right Vertex
    m_Vertices[3].set(MaxX, MaxY, 0.0f, 1.0f);
    m_Vertices[3].SetNormal(0.0f, 0.0f, 1.0f);
    m_Vertices[3].SetUV(1.0f, 1.0f);

    for(int i = 0; i < BLIT_VERTEX_COUNT; i++)
        m_Vertices[i].SetColor((unsigned char)0xFF, (unsigned char)0xFF, (unsigned char)0xFF, (unsigned char)0xFF);

Error:
    return r;
}

RESULT blit::ConvertToScreen() {
    RESULT r = R_SUCCESS;

    CBRM(m_Screen_f, "blit: Convert to screen called before screen specs set!");
    
    m_ptOrigin = m_ptOrigin.ToScreenCoordinates(m_ScreenWidth, m_ScreenHeight);
    m_Height = (float)(m_Height / (float)(m_ScreenHeight/2.0f));
    m_Width = (float)(m_Width / (float)(m_ScreenWidth/2.0f));

Error:
    return r;
}

float blit::GetWidth(){ return m_Width; }    
float blit::GetHeight(){ return m_Height; }

RESULT blit::SetWidth(float width) { 
    RESULT r = R_SUCCESS;
    
    if(width != m_Width)  {
        m_Width = width; 
        CR(ConvertToScreen());
        CR(CreateBlitVertices());
    }

Error:
    return r; 
}

RESULT blit::SetHeight(float height) { 
    RESULT r = R_SUCCESS;
    
    if(height != m_Height) {
        m_Height = height; 
        CR(ConvertToScreen());
        CR(CreateBlitVertices());
    }

Error:
    return r; 
}

RESULT blit::SetWidthHeight(float width, float height) { 
    RESULT r = R_SUCCESS;
    
    if(width != m_Width || height != m_Height) {
        m_Width = width; 
        m_Height = height;
        CR(ConvertToScreen());
        CR(CreateBlitVertices());
    }

Error:
    return r; 
}

BLIT_TYPE blit::GetBlitType(){ return m_type; }

const char* blit::ToString() {
    return "Blit";
}

RESULT blit::InitializeFlatObject(void *pContext) {
    printf("Initialize Generic Flat Object\n");
    return R_NOT_IMPLEMENTED;
}

RESULT blit::Render(DimensionImp *pDimImp) {
    return R_NOT_IMPLEMENTED;
}

RESULT blit::Initialize(DimensionImp *pDimImp) {
    return R_NOT_IMPLEMENTED;
}

vertex* blit::GetVertexBuffer() {
    return m_Vertices;
}

int blit::GetVertexCount() {
    return BLIT_VERTEX_COUNT;
}

