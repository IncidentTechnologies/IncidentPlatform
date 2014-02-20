#pragma once

// 3DText is a buffer object as well as a 3D Mesh implementation. On the dimension
// level it is very similar to the text object but differs in that it contains 
// descriptors in regards to depth, and other things.

#include "EHM.h"
#include "DimObject.h"

#define DEFAULT_TEXT3D_BUFFER_SIZE 100

#define DEFAULT_FONT3D_NAME "Arial"
#define DEFAULT_FONT3D_HEIGHT 0.5f
#define DEFAULT_FONT3D_WIDTH 0
#define DEFAULT_FONT3D_DEPTH 0.2f

struct Font3D {
    char *name;
    int height;
    int width;
    float depth;
};

using namespace dss;

class Text3D : public DimObject {

public: 
    Text3D(point ptOrigin) :
        DimObject(ptOrigin, DIM_TEXT3D),
        m_Location_c(0)
    {
        RESULT r = Initialize(NULL);
    }

    Text3D(point ptOrigin, const char* c_str) :
	  DimObject(ptOrigin, DIM_TEXT3D),
	  m_Location_c(0)
	{
		RESULT r = Initialize(c_str);
	}

	~Text3D(){/*empty stub*/}

public:
    static Font3D *InitFont3D();

	char *GetBuffer(){ return m_pBuffer; }

    const char* GetFontName(){ return m_pFont->name;}
	
    float GetFontHeight(){ return m_pFont->height;}
    RESULT SetFontHeight(float h){ m_pFont->height = h; return R_SUCCESS; }
	float GetFontWidth(){ return m_pFont->width;}
    RESULT SetFontWidth(float w){ m_pFont->width = w; return R_SUCCESS; }

    
    float GetFontDepth(){ return m_pFont->depth;}

    const char *ToString()  {
        return "3D Text";
    }

    // 3d Text object has no vertex data for now
    vertex* GetVertexBuffer() {
        return NULL;
    }
    
    int GetVertexCount() {
        return 0;
    }
    

private:
	RESULT Initialize(const char* c_str);

public:
    char *m_pBuffer;


private:
    int m_Location_c;
    Font3D *m_pFont;

};