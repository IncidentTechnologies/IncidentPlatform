#ifdef __WIN32__
#pragma once

// DX9 Implementation of plane
// A plane is a flat set of points 

//#include "DimensionImpDX9.h"

#include <d3d9.h>
#include <d3dx9.h>
#include "RESULT.h"
#include "EHM.h"

#include "point.h"
#include "DX9DimObject.h"
#include "plane.h"


class DimensionImpDX9;

class DX9Plane :
    public plane,
    public DX9DimObject
{
public:
    DX9Plane() :
      plane(),
          m_pd39Buf(NULL),
          m_pIndexBuffer(NULL),
          m_indices(NULL)
      {

      }

      DX9Plane(DimObject *obj) :
      plane(obj->m_ptOrigin, 
            static_cast<plane*>(obj)->GetWidth(),
            static_cast<plane*>(obj)->GetLength(),
            static_cast<plane*>(obj)->GetTexReps()),
          m_pd39Buf(NULL),
          m_pIndexBuffer(NULL),
          m_indices(NULL)
      {

      }

      ~DX9Plane()
      {
          m_pd39Buf->Release();    
          m_pIndexBuffer->Release();
      }

public:
    HRESULT DX9Init(LPDIRECT3DDEVICE9 pd3device9);
    HRESULT DX9Update();
    RESULT Render(DimensionImpDX9 *pDimImpDX9);
    RESULT Render(LPDIRECT3DDEVICE9 pd3device9);

public:
    IDirect3DVertexBuffer9* m_pd39Buf;
    IDirect3DIndexBuffer9* m_pIndexBuffer;
    int *m_indices;
};

#endif // #ifdef __WIN32__
