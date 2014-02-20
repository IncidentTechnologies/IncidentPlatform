#ifdef __WIN32__
#pragma once

// The DX9 Blit is effectively a texture mapped quad

#include "DX9FlatObject.h"
#include "blit.h"

#include "DimensionImpDX9.h"

class DX9Blit :
    public blit,
    public DX9FlatObject
{
public:
    DX9Blit();
    DX9Blit(point origin, float height, float width, BLIT_TYPE type);
    DX9Blit(FlatObject *obj);
    ~DX9Blit();

public:
    RESULT Initialize(DimensionImp* pDimImp);
    RESULT Update();
    RESULT Render(DimensionImp* pDimImp);
    RESULT Render(LPDIRECT3DDEVICE9 pd3device9);

public:
    IDirect3DVertexBuffer9* m_pd39Buf;
    IDirect3DVertexDeclaration9 *m_pVertexDecl;

    // SHADERS
    IDirect3DVertexShader9 *m_pVertexShader;
    ID3DXConstantTable *m_pVertexConstants;

    IDirect3DPixelShader9 *m_pPixelShader;
    ID3DXConstantTable *m_pPixelConstants;

    ID3DXBuffer *m_pTempBuffer;
    // **

    IDirect3DTexture9 **m_ppTextures;    

};

#endif // #ifdef __WIN32__
