#ifdef __WIN32__

#pragma once

// DirectX9 Implementation of the dimension text object
// a noteable difference between this object and the other
// directX implementations is that it requires no vertex buffer

// BIG TODOs: Update the text mesh here

class DX93DText :
	public Text3D,
	public DX9DimObject
{
public:
	DX93DText(point ptOrigin) :
	  Text3D(ptOrigin),
	  m_pTextMesh(NULL)
	{
		
	}

	DX93DText(point ptOrigin, const char* c_str) :
	  Text3D(ptOrigin, c_str),
	  m_pTextMesh(NULL)
	{

	}

	DX93DText(DimObject *obj) :
	  Text3D(obj->m_ptOrigin, static_cast<Text3D*>(obj)->GetBuffer()),
	  m_pTextMesh(NULL)
	{

	}

	~DX93DText()
	{
		if(m_pTextMesh != NULL)
		{
			m_pTextMesh->Release();
            m_pTextMesh = NULL;
		}
	}
    
	HRESULT DX9Init(LPDIRECT3DDEVICE9 pd3device9)
	{
        HRESULT hr = S_OK;
        HDC hdc;
        HFONT hFont;

        hdc = CreateCompatibleDC(NULL);

        hFont = CreateFont( GetFontHeight(),
                            GetFontWidth(),
                            0,                              // escapement
                            0,                              // orientation
                            FW_NORMAL,                      // weight
                            false,                          // itallics
                            false,                          // underline
                            false,                          // strikeout
                            DEFAULT_CHARSET,                // charset
                            OUT_DEFAULT_PRECIS,             // precision
                            CLIP_DEFAULT_PRECIS,            // clipping precision
                            DEFAULT_QUALITY,                // quality
                            DEFAULT_PITCH || FF_DONTCARE,   // pitch and family
                            //(LPCWSTR) GetFontName()
                            TEXT("Arial")
                           );

        HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);

        // Convert Buffer to a wide string
        wchar_t *wText = new wchar_t[strlen(GetBuffer()) + 1];
        memset(wText, 0, strlen(GetBuffer()) + 1);
        ::MultiByteToWideChar(CP_ACP, NULL, GetBuffer(), -1, wText, strlen(GetBuffer()) + 1);

        hr = D3DXCreateText(   pd3device9,
                                hdc,
                                wText,
                                0.001f,                     // Maximum chordal deviation from true font outlines
                                GetFontDepth(),             // Extrusion Depth (z axis)
                                &m_pTextMesh,               // ID3DXMesh 
                                NULL,                       // Adjacency info
                                NULL                        // GlyphMetrics
                            );
        
        DeleteObject(hFontOld);
        DeleteObject(hFont);
        DeleteDC(hdc);

		return hr;
    }

	HRESULT DX9Update()
	{
        HRESULT hr = S_OK;
        void* pVoid;
        D3DXMATRIX matTemp;

        D3DXMatrixIdentity(&m_matTransform);
        
        if( m_fScale != 1.0f )
        {   
            // Scale only if scale is not 1
            D3DXMatrixScaling(&matTemp, m_fScale, m_fScale, m_fScale);
            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }

        if( m_fRotationX != 0.0f )
        {
            D3DXMatrixRotationX(&matTemp, m_fRotationX);
            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }
        if(m_fRotationY != 0.0f)
        {
            D3DXMatrixRotationY(&matTemp, m_fRotationY);
            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }
        if(m_fRotationZ != 0.0f )
        {
            D3DXMatrixRotationZ(&matTemp, m_fRotationZ);
            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }
        if( m_ptOrigin != point(0.0f, 0.0f, 0.0f))
        {
            // Translate only if not at origin anyways
            D3DXMatrixTranslation(&matTemp, m_ptOrigin.tuple[DIM_X], 
                                            m_ptOrigin.tuple[DIM_Y], 
                                            m_ptOrigin.tuple[DIM_Z]);

            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }

        return hr;
	}

    RESULT Render(DimensionImpDX9* pDimImpDX9)
    {
        return R_NOT_IMPLEMENTED;
    }

	RESULT Render(LPDIRECT3DDEVICE9 pd3device9)
	{
	    RESULT r = R_OK;

        HRESULT hr = S_OK;
  
        // Update Vertices
        hr = DX9Update();
        CHR(hr);

        // Set Worldview Matrix
        pd3device9->SetTransform(D3DTS_WORLD, &(m_matTransform));

        m_pTextMesh->DrawSubset(0);

Error:
        return r;
    }

private:
    ID3DXMesh *m_pTextMesh;
};

#endif // #ifdef __WIN32__