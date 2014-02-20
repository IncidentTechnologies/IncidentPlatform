#ifdef __WIN32__
#pragma once

// DirectX9 Implementation of the dimension text object
// a noteable difference between this object and the other
// directX implementations is that it requires no vertex buffer

#include "DX9FlatObject.h"

#include "DimensionImpDX9.h"

class DX9Text :
	public text,
	public DX9FlatObject
{
public:
	DX9Text(point ptOrigin);
	DX9Text(point ptOrigin, const char* c_str);
	DX9Text(FlatObject *obj);
	~DX9Text();
    
    // DX9FlatObject
	RESULT Initialize(DimensionImp *pDimImp);
	RESULT Update();
    RESULT Render(DimensionImp* pDimImp);
	RESULT Render(LPDIRECT3DDEVICE9 pd3device9);

    // text
    blit GetBlit();

    // DX9Text
    RESULT InitializeFont();        // This code is repeated in the initialization function and Update functions

private:
    LPD3DXFONT m_dxfont;
    blit m_blitText;                    // updated with the text position and rectangle
    LPDIRECT3DDEVICE9 m_pd3device9;    // handle to the directx9 device
};

#endif // #ifdef __WIN32__
