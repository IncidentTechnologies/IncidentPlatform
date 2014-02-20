#pragma once

// The text object is effectively a buffer that can be edited and updated
// The idea for this is that this buffer is then linked with a font object to be rendered.
// It is important to note here that a text object implies no 3D aspects to the text.  
// If you would like to create text with 3d attributes you should use the 3DText primitive
// as text implies no mesh and 3Dtext implies a mesh.

#define DEFAULT_TEXT_BUFFER_SIZE 100

#include "FlatObject.h"
#include "RESULT.h"
#include "EHM.h"
#include "blit.h"
#include "font.h"

#include "SmartBuffer.h"

class text :
	public FlatObject
{
public:
	text(point ptOrigin, const char* c_pszString = NULL, font *pFont = NULL) :
	  FlatObject(ptOrigin, FLAT_TEXT),
	  m_Location_c(0),
      m_pFont(pFont)      
	{
		RESULT R = InitializeText(c_pszString);
	}

	~text()
    {
        if(m_pSmartBuffer != NULL)
        {
            delete m_pSmartBuffer;
            m_pSmartBuffer = NULL;
        }
    }	

    const char* ToString()
    {
        return "Text";
    }

	const char* GetFontName(){ return m_pFont->m_pszName;}
	int GetFontHeight(){ return m_pFont->m_Height;}
	int GetFontWidth(){ return m_pFont->m_Width;}
    int GetFontWeight(){ return m_pFont->m_Weight; }
    bool GetFontItalics(){ return m_pFont->m_Italics; }

    RESULT ConvertToScreen()
    {
        // Using DX for text right now only
        // should eventually fix this
        return R_UNSUPPORTED;
    }

    color GetFontColor(){ return m_pFont->m_Color; }
    RESULT SetFontColor(color c){ m_pFont->m_Color = c; return R_SUCCESS; }

    font *GetFont(){ return m_pFont; }
    RESULT SetFont(font *pFont)
    { 
        m_pFont = pFont; 
        return R_SUCCESS; 
    }

    RESULT InitializeFlatObject(void *pContext)
    {
        printf("Initialize Generic Flat Object\n");
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

    // Implemented by dimension implmentation object
    // since size of text may change with implementation
    virtual blit GetBlit()
    {
        return blit(point(), 0.0f, 0.0f, BLIT_SCREEN);
    }

    /*const char* GetText()
    { 
        return m_pSmartBuffer->GetBuffer(); 
    }*/

    const char* GetBuffer()
    {
        return m_pSmartBuffer->GetBuffer();
    }

    SmartBuffer *GetSmartBuffer()
    { 
        return m_pSmartBuffer; 
    }

    // Set Text will erase everything in the buffer and replace it with a 
    // new buffer  (this will delete the old memory and create a new buffer)
    RESULT SetBuffer(const char *pszBuffer)
    {
        RESULT r = R_SUCCESS;

        CRM(m_pSmartBuffer->ResetBuffer(pszBuffer), "SetBuffer: Failed to Reset the buffer");

Error:
        return r;
    }

    // Text has no vertex information for the time being
    vertex* GetVertexBuffer()
    {
        return NULL;
    }
    int GetVertexCount()
    {
        return 0;
    }

private:
	RESULT InitializeText(const char* c_pszString)
	{
		RESULT r = R_SUCCESS;      

        if(m_pFont == NULL)
        {
		    m_pFont = InitializeFontDefault();
        }

        m_pSmartBuffer = new SmartBuffer(c_pszString);
        CNRM(m_pSmartBuffer, "InitializeText: Failed to allocate SmartBuffer");

Error:
		return R_SUCCEED;
	}

public:
    SmartBuffer *m_pSmartBuffer;

private:
	int m_Location_c;
	font *m_pFont;

public:
	static font *InitializeFontDefault()
	{
		struct font *ret = new font();

		ret->m_pszName = new char[strlen(DEFAULT_FONT_NAME)];
		strcpy(ret->m_pszName, DEFAULT_FONT_NAME);

		ret->m_Height = DEFAULT_FONT_HEIGHT;
		ret->m_Width = DEFAULT_FONT_WIDTH;

		return ret;
	}
};