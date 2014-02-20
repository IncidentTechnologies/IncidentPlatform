#ifdef __WIN32__
#pragma once

// The DX9 Blit is effectively a texture mapped quad

#include "DX9FlatObject.h"
#include "line.h"

#include "DimensionImpDX9.h"

class DX9Line :
    public line,
    public DX9FlatObject
{
public:
    DX9Line(point ptOrigin, point ptEnd, float LineWidth);
    DX9Line(FlatObject *obj);
    ~DX9Line();

public:
    RESULT Initialize(DimensionImp* pDimImp);
    RESULT Update();
    RESULT Render(DimensionImp* pDimImp);
    RESULT Render(LPDIRECT3DDEVICE9 pd3device9);

public:
    IDirect3DVertexBuffer9* m_pd39Buf;
    IDirect3DVertexDeclaration9 *m_pVertexDecl;
    // SHADERS
    // Vertex
    IDirect3DVertexShader9 *m_pVertexShader;
    ID3DXConstantTable *m_pVertexConstants;
    // Pixel
    IDirect3DPixelShader9 *m_pPixelShader;
    ID3DXConstantTable *m_pPixelConstants;
    // Buffer
    ID3DXBuffer *m_pTempBuffer;
    // Textures
    IDirect3DTexture9 **m_ppTextures;    
};

#endif // #ifdef __WIN32__
